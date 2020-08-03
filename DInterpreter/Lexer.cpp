#include "Lexer.h"

namespace inter
{

Lexer::Lexer(unsigned char* stringstream) : sInputStream(stringstream),iPosition(0),iexpr(0),streampos(0),prelexed(false)
{
}

Lexer::~Lexer(void)
{
}

// Tokenliste erstellen auf Basis des Inputstreams erstellen
void Lexer::prelex() 
{
	while(!isready())
		tokenstream.push_back(getnexttoken());
	prelexed=true; 
}

token* Lexer::getnexttoken()
{
	char cCurrent = sInputStream[iPosition];
	// auf Whitespace überprüfen und gegebenenfalls überspringen
	if(cCurrent == ' ' || cCurrent == '\r' || cCurrent == '\t' || cCurrent == '\n')
	{
		iPosition++;
		return getnexttoken(); 
	}
	// auf Spezialfälle die länger als ein Zeichen sind testen
	switch (cCurrent) 
	{
	case '/':
		{
			iPosition++;
			if(sInputStream[iPosition] == '/') // Kommentarzeile
				while(sInputStream[iPosition] != '\n')
					iPosition++;
			iPosition++;
			return getnexttoken();
			break;
		}
	case '{':
		{
			iPosition++;
			while(sInputStream[iPosition] != '}') //Kommentarblock { block }
				iPosition++;
			iPosition++;
			return getnexttoken();
			break;
		}
	case '\'': // Stringliteral ' text '
		{
			iPosition++;
			stringlit* slit = new stringlit(); 
			while(sInputStream[iPosition] != '\'') // IO-Read Fehler am Ende des Zeichenstreams bei ungeschlossenem Stringliteral
			{
				slit->append(sInputStream[iPosition]);
				iPosition++;
			}
			iPosition++;
			return slit;
			break;
		}
	case ':':
		iPosition++;
		if(sInputStream[iPosition] == '=') // Zuweisungsoperator :=
		{
			iPosition++;
			return new token(ttypes::ASSIGN);
		}
		else
			return new token(static_cast<int>(':'));
		break;
	case '<':
		iPosition++;
		if(sInputStream[iPosition] == '=') // Kleinergleich-Operator <= 
		{
			iPosition++;
			return new token(ttypes::LESS_EQUALS);
		}
		else if(sInputStream[iPosition] == '>') // Ungleich-Operator <>
		{
			iPosition++;
			return new token(ttypes::UNEQUALS);
		}
		else
			return new token(static_cast<int>('<'));
		break;
	case '>':
		iPosition++;
		if(sInputStream[iPosition] == '=') // Größergleich-Operator
		{
			iPosition++;
			return new token(ttypes::GREATER_EQUALS);
		}
		else
			return new token(static_cast<int>('>'));
		break;
	}
	//auf Textsymbole prüfen
	// erstes Zeichen muss ein Buchstabe oder Unterstrich sein
	if(isalpha(cCurrent) || cCurrent == '_') 
	{
		identifier* id = new identifier(static_cast<char>(tolower(cCurrent)));
		iPosition++;
		while(!isready())
			//Buchstaben oder Unterstrich akzeptiert
			if(isalnum(sInputStream[iPosition]) || sInputStream[iPosition] == '_') 
				id->append(tolower(sInputStream[iPosition++]));
			else
				break;
		id->checkkeyword(); // auf Schlüsselwort überprüfen
		return id;
	}
	// auf Zahl prüfen
	else if(isdigit(cCurrent))
	{
		int start = iPosition;
		bool isReal = false;
		iPosition++;
		while(!isready())
			if(isdigit(sInputStream[iPosition]))
				iPosition++;
			else
				break;
		if(!isready())
			if(sInputStream[iPosition] == '.') // Gleitkommazahl weil ein Punkt gefunden wurde
			{				
				isReal = true;
				iPosition++;
				while(!isready())
					if(isdigit(sInputStream[iPosition]))
						iPosition++;
					else
						break;
			}
			char* buffer = new char[iPosition-start+1];
			// String erzeugen der die Zahl speichert
			for(unsigned int i = 0; i < iPosition-start; i++)
				buffer[i] = sInputStream[start+i]; 
			buffer[iPosition-start] = '\0';
			if(isReal)
				return new real(static_cast<REALNUM>(atof(buffer)));
			else
				return new integer(static_cast<int>(atoi(buffer)));
	}
	else
	{
		iPosition++;
		return new token(static_cast<int>(cCurrent)); // anderes Zeichen als token zurückgeben
	}
	return NULL;
}

} // namespace