#include "Section.h"

SectionToken::SectionToken(string& s)
{
  sectionText = s;
}

Section::Section(SectionToken& st)
{
  text = st.getText();
  type = NULLTYPE;
}

Link::Link(SectionToken& stok) : Section(stok)
{
  secIndex = 0;
  type = LINK;
  if(stok.getText().find("-&gt;", secIndex) != string::npos)
  {
    secIndex = stok.getText().find("-&gt;", secIndex);
    text = stok.getText().substr(2, secIndex - 2);
    secIndex += 5;
    passName = stok.getText().substr(secIndex, stok.getText().length() - secIndex - 2);
  }
  else{
    text = stok.getText().substr(2, stok.getText().length() - 4);
    passName = text;
  }
}

Text::Text(SectionToken& stok) : Section(stok)
{
  type = TEXT;
}

Goto::Goto(SectionToken& stok) : Section(stok)
{
  secIndex = stok.getText().find("&quot;") + 6;
  type = GOTO;
  passName = stok.getText().substr(secIndex, stok.getText().find("&quot;", secIndex) - secIndex);
}

Set::Set(SectionToken& stok) : Section(stok)
{
  secIndex = stok.getText().find("$") + 1;
  type = SET;
  text = stok.getText().substr(secIndex, stok.getText().find(" to ", secIndex) - secIndex);

  secIndex = stok.getText().find(" to ", secIndex) + 4;
  if(stok.getText().substr(secIndex, 4) == "true"){
    value = true;
  }
  else{
    value = false;
  }
}

If::If(SectionToken& stok) : Section(stok)
{
  secIndex = stok.getText().find("$") + 1;
  type = IF;
  text = stok.getText().substr(secIndex, stok.getText().find(" is ", secIndex) - secIndex);

  secIndex = stok.getText().find(" is ", secIndex) + 4;
  if(stok.getText().substr(secIndex, 4) == "true"){
    valueToCheck = true;
  }
  else{
    valueToCheck = false;
  }
}

Elseif::Elseif(SectionToken& stok) : Section(stok)
{
  secIndex = stok.getText().find("$") + 1;
  type = ELSEIF;
  text = stok.getText().substr(secIndex, stok.getText().find(" is ", secIndex) - secIndex);

  secIndex = stok.getText().find(" is ", secIndex) + 4;
  if(stok.getText().substr(secIndex, 4) == "true"){
    valueToCheck = true;
  }
  else{
    valueToCheck = false;
  }
}

Else::Else(SectionToken& stok) : Section(stok)
{
  type = ELSE;
}

bool BlockTokenizer::hasNextSection(Block& bl)
{
  if (bl.getText().find("(set:", bl.blIndex) != string::npos) {
		return true;
	}
	else if (bl.getText().find("(go-to:", bl.blIndex) != string::npos) {
		return true;
	}
	else if (bl.getText().find("(if:", bl.blIndex) != string::npos) {
		return true;
	}
	else if (bl.getText().find("(else-if:", bl.blIndex) != string::npos) {
		return true;
	}
	else if (bl.getText().find("(else:", bl.blIndex) != string::npos) {
		return true;
	}
	else if (bl.getText().find("[[", bl.blIndex) != string::npos) {
		return true;
	}
	else if (bl.getText().substr(bl.blIndex, 1) != "<") {
		return true;
	}
	else {
		return false;
	}
}

void BlockTokenizer::nextSection(Block& bl)
{
  int sectionBeginning;
  string stokenText;
  bl.blIndex += 2;

  //test to see if the next section is a Text Section.
  if ((bl.getText().substr(bl.blIndex, 1) != "(") && (bl.getText().substr(bl.blIndex, 1) != "["))
  {
    sectionBeginning = bl.blIndex;

    if ((bl.getText().find("(", bl.blIndex) == string::npos) && (bl.getText().find("[", bl.blIndex) == string::npos))
    {
      bl.blIndex = bl.getText().find("<", bl.blIndex);
    }
    else if (bl.getText().find("(", bl.blIndex) < bl.getText().find("[", bl.blIndex))
    {
      bl.blIndex = bl.getText().find("(", bl.blIndex);
    }
    else
    {
      bl.blIndex = bl.getText().find("[", bl.blIndex);
    }
    stokenText = bl.getText().substr(sectionBeginning, bl.blIndex - sectionBeginning);
    SectionToken stok(stokenText);
    Text text(stok);
    bl.addSection(text);
  }
  else if(bl.getText().substr(bl.blIndex, 3) == "[[[")
  {
    int counter = 1;
    sectionBeginning = bl.blIndex;
    bl.blIndex++;
    while(counter != 0){
      if(bl.getText().at(bl.blIndex) == '['){
        counter++;
      }
      if(bl.getText().at(bl.blIndex) == ']'){
        counter--;
      }
      bl.blIndex++;
    }
    stokenText = bl.getText().substr(sectionBeginning, bl.blIndex - sectionBeginning);
    SectionToken stok(stokenText);
    bl.addSection(block);
  }
  else if (bl.getText().substr(bl.blIndex, 5) == "(set:")
  {
    sectionBeginning = bl.getText().find("(set:", bl.blIndex);
    bl.blIndex = bl.getText().find(")", sectionBeginning) + 1;
    stokenText = bl.getText().substr(sectionBeginning, bl.blIndex - sectionBeginning);
    SectionToken stok(stokenText);
    bl.addSection(set);
  }
  else if (bl.getText().substr(bl.blIndex, 7) == "(go-to:")
  {
    sectionBeginning = bl.getText().find("(go-to:", bl.blIndex);
    bl.blIndex = bl.getText().find(")", sectionBeginning) + 1;
    stokenText = bl.getText().substr(sectionBeginning, bl.blIndex - sectionBeginning);
    SectionToken stok(stokenText);
    bl.addSection(goto);
  }
  else if (bl.getText().substr(bl.blIndex, 4) == "(if:")
  {
    sectionBeginning = bl.getText().find("(if:", bl.blIndex);
    bl.blIndex = bl.getText().find(")", sectionBeginning) + 1;
    stokenText = bl.getText().substr(sectionBeginning, bl.blIndex - sectionBeginning);
    SectionToken stok(stokenText);
    bl.addSection(ifSec);
  }
  else if (bl.getText().substr(bl.blIndex, 9) == "(else-if:")
  {
    sectionBeginning = bl.getText().find("(else-if:", bl.blIndex);
    bl.blIndex = bl.getText().find(")", sectionBeginning) + 1;
    stokenText = bl.getText().substr(sectionBeginning, bl.blIndex - sectionBeginning);
    SectionToken stok(stokenText);
    bl.addSection(elseif);
  }
  else if (bl.getText().substr(bl.blIndex, 6) == "(else:")
  {
    sectionBeginning = bl.getText().find("(else:", bl.blIndex);
    bl.blIndex = bl.getText().find(")", sectionBeginning) + 1;
    stokenText = bl.getText().substr(sectionBeginning, bl.blIndex - sectionBeginning);
    SectionToken stok(stokenText);
    bl.addSection(el);
  }
  else if (bl.getText().substr(bl.blIndex, 2) == "[[")
  {
    sectionBeginning = bl.getText().find("[[", bl.blIndex);
    bl.blIndex = bl.getText().find("]]", sectionBeginning) + 2;
    stokenText = bl.getText().substr(sectionBeginning, bl.blIndex - sectionBeginning);
    SectionToken stok(stokenText);
    bl.addSection(link);
  }
  else
  {
    int counter = 1;
    sectionBeginning = bl.blIndex;
    bl.blIndex++;
    while(counter != 0)
    {
      if(bl.getText().at(bl.blIndex) == '[')
      {
        counter++;
      }
      if(bl.getText().at(bl.blIndex) == ']')
      {
        counter--;
      }
      bl.blIndex++;
    }
    stokenText = bl.getText().substr(sectionBeginning, bl.blIndex - sectionBeginning);
    SectionToken stok(stokenText);
    bl.addSection(block);
  }
}

Block::Block(SectionToken& stok) : Section(stok)
{
  type = BLOCK;
  blIndex = 2;
  BlockTokenizer btkzr;

  while(btkzr.hasNextPassage(*this))
  {
    btkzr.nextPassage(*this);
  }
}

void Block::addSection(Section* blockSect)
{
  .getText().push_back(blockSect);
}

bool Block::startBlock(vector<pair<string, string>>& listOfLinks, int gotoIndex, bool& gotoExists, string& passName)
{
  bool ifElseIfElse = true;

  if(gotoExists == false)
  {
    for(int i = 0; i < .getText().size(); i++)
  	{
  		if(.getText().at(i).getType() == GOTO)
  		{
  			gotoIndex = i;
  			gotoExists = true;
  			passName = .getText().at(i).getPassName();
        break;
  		}
  		else
  		{
  			gotoIndex = .getText().at(i).getType();
  		}
  	}
  }

  for(int i = 0; i < gotoIndex; i++)
  {
    type_t currentType = .getText().at(i).getType();
    string currentText = .getText().at(i).getText();

    if(currentType == SET)
    {
      addVariable(currentText, .getText().at(i).getValue());
    }
    else if(currentType == TEXT)
    {
      cout << currentText << endl;
    }
    else if(currentType == LINK)
    {
			if(gotoExists == false)
			{
				passName = .getText().at(i).getPassName();

	      cout << "\"" + currentText + "\"" << endl;
	      listOfLinks.push_back(make_pair(currentText, passName));
			}
    }
    else if(currentType == IF)
    {
      if(.getText().at(i).getValueToCheck() == getVarVal(currentText))
      {
        ifElseIfElse = false;
      }
      else
      {
        i++;
      }
    }
    else if(currentType == ELSEIF)
    {
      if(.getText().at(i).getValueToCheck() == getVarVal(currentText) && ifElseIfElse == true)
      {
        ifElseIfElse = false;
      }
      else
      {
        i++;
      }
    }
    else if(currentType == ELSE)
    {
      if(ifElseIfElse == false)
      {
        i++;
      }
    }
    else
    {
			.getText().at(i).startBlock(listOfLinks, gotoIndex, gotoExists, passName);
    }
  }
}
