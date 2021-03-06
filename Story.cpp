#include "Story.h"

bool StoryTokenizer::hasNextPassage(Story& theStory)
{
	return theStory.storyText.find("<tw-passagedata ", index) != string::npos;
}

void StoryTokenizer::nextPassage(Story& theStory)
{
	int passageBeginning;

	passageBeginning = theStory.storyText.find("<tw-passagedata ", index);
	index = theStory.storyText.find("</tw-passagedata>", passageBeginning) + 17;

	PassageToken ptok(theStory.storyText.substr(passageBeginning, index - passageBeginning));
	Passage p(ptok);

	theStory.passages.push_back(p);
}



Story::Story(string text)
{
	storyText = text;
}



void Story::addLookup(string& name, int& index)
{
	lookUpPassage[name] = index;
}



void Story::addVariable(string& varName, bool& value)
{
	variables[varName] = value;
}



bool Story::getVarVal(string& varName) const
{
	return variables.at(varName);
}



int Story::lookup(string& passName) const
{
	return lookUpPassage.find(passName)->second;
}



void Story::startPassage(int index)
{
  bool ifElseIfElse = true;
	bool gotoExists = false;
	int j, chosen;
	string passName;

	listOfLinks.clear();

	for(int i = 0; i < passages.at(index).getSec().size(); i++)
	{
		if(passages.at(index).getSec().at(i).getType() == GOTO)
		{
			j = i;
			gotoExists = true;
			passName = passages.at(index).getSec().at(i).getPassName();
			break;
		}
	}

	if(gotoExists == false)
	{
		j = passages.at(index).getSec().size();
	}

  for(int i = 0; i < j; i++)
  {
    type_t currentType = passages.at(index).getSec().at(i).getType();
    string currentText = passages.at(index).getSec().at(i).getText();

    if(currentType == SET)
    {
			bool value = passages.at(index).getSec().at(i).getValue();
      addVariable(currentText, value);
    }
    else if(currentType == TEXT)
    {
      cout << currentText << endl;
    }
    else if(currentType == LINK)
    {
			if(gotoExists == false)
			{
				passName = passages.at(index).getSec().at(i).getPassName();
	      cout << "\"" + currentText + "\"" << endl;
	      listOfLinks.push_back(make_pair(currentText, passName));
			}
    }
    else if(currentType == IF)
    {
      if(passages.at(index).getSec().at(i).getValueToCheck() == getVarVal(currentText))
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
      if(passages.at(index).getSec().at(i).getValueToCheck() == getVarVal(currentText) && ifElseIfElse == true)
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
			passages.at(index).getSec().at(i).startBlock(variables, listOfLinks, j, gotoExists, passName);
    }

  }

	if(listOfLinks.size() > 0)
	{
		for(int i = 0; i < listOfLinks.size(); i++)
		{
			cout << (i + 1) << ". " << listOfLinks.at(i).first << endl;
		}

		cin >> chosen;
		chosen--;
		passName = listOfLinks.at(chosen).second;

	}

	chosen = lookup(passName);

	startPassage(chosen);
}
