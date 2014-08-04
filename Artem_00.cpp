#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "algorithm"
#include <vector>
#include <set>
#include <iomanip>
#include <stack> 
#include <stdio.h>

using namespace std;

//#define PATH "D:/Catalog/target_specs/nikon_d5200.html"
//#define PATH "D:/Catalog/RandomWeb/ttttt.txt"


//#define PATH "D:/Catalog/RandomWeb/EpsonArtisan50_full.html"
//#define PATH "D:/Catalog/RandomWeb/kohlHelicopter.html"
//#define PATH "D:/Catalog/RandomWeb/TargetHelicopter.html"
//#define PATH "D:/Catalog/RandomWeb/ToysRUsHelicopter.html"
//#define PATH "D:/Catalog/RandomWeb/AmazonHelicopter.html"

//#define PATH "D:/Catalog/RandomWeb/TacticalBackpacks.htm"
//#define PATH "D:/Catalog/RandomWeb/CanonPowerShotS100.htm"
#define PATH "D:/Catalog/RandomWeb/Telescope.htm"
//#define PATH "D:/Catalog/RandomWeb/RakutenNikon.htm"
//#define PATH "D:/Catalog/RandomWeb/WA_Washer.htm"


class Collect{
	int x;
	int numberOfElements;
	//int numberOfElements;

	map <int,string> dataRecords;
	map <int,string> htmlRecords;

	//static int frequency [256];



	int asciiCharacter;

	int dataLine;
	int lineCount;
	vector <vector <int> > frequency;
	
	vector <vector <int> > freqHtmlAlligned;
	vector <vector <int> > freqDataAlligned;
	vector <string> dataAlligned;
	

public:
	Collect(){
		x=0;
		numberOfElements=256;
		lineCount=-1;
		//numberOfElements(256);
		
		
	}

	void trim(std::string & str)
	{
		std::string trim_chars = " \t\n\v\f\r";
		std::string::size_type pos = str.find_last_not_of(trim_chars);
		if(pos != std::string::npos) {
			str.erase(pos + 1);
			pos = str.find_first_not_of(trim_chars);
			if(pos != std::string::npos) str.erase(0, pos);
		}
		else
			str.erase(str.begin(), str.end());
	}

	void addRow(int elementsInRow, vector <vector <int> > & VectToExpand, int& count){
		count++;
		VectToExpand.push_back( vector<int>(elementsInRow,0) ) ;
	}

	void getData(string path){
		string line;
		ifstream inFile (path.c_str());
	
		bool html=false;

		if (inFile.is_open())		
		{
			
			while ( getline (inFile,line) )
			{	
				if (lineCount>290){
					x=0;
					//break;
				}
				
				trim(line);
				
				if ( line.length()==0 ){
					continue;
				}

				string data="";
				for(size_t i=0 ; i<line.length(); i++)
				{

					if( html==false && line[i]=='<' ){
						html=true;
						addRow( numberOfElements,frequency,lineCount);

						//cout<<lineCount<<"	";

					}
					if( html==true && line[i]=='>' ){
						html=false;
					}
					
					if ( html==false  && line[i]!='>'){

						if( (i>0 && line[i-1]=='>') || i==0 ){
							trim(data);
							if ( data.length()>0){
								//cout<<endl<<"						--->	" <<data<<endl;

							}

							addRow(numberOfElements,frequency,lineCount);
							dataLine=lineCount;
							data="";
							//cout<<endl<<lineCount<<"--------------------------------------_____"<<endl;;
						}
						if( 0<=(int)line[i] && (int)line[i]<256  ){
							data+=line[i];
							asciiCharacter=(int)line[i];
						}
						else{
							//cout<<endl<<line[i]<<" is NOT ASCI "<< (int)line[i]<<endl;
							data+=' ';
							asciiCharacter=(int)' ';
						}
						dataRecords[dataLine]+=(char)asciiCharacter;
						frequency[dataLine][asciiCharacter]++;						
					}	
					else{
						//cout <<line[i];
						if( 0<=(int)line[i] && (int)line[i]<256  ){
							asciiCharacter=(int)line[i];
						}
						else{

							//cout<<endl<<line[i]<<" is NOT ASCI "<< (int)line[i]<<endl;
							asciiCharacter=' ';
						}
						htmlRecords[lineCount]+=asciiCharacter;
						frequency[lineCount][asciiCharacter]++;
					}
											

					


				}
				//cout<<"			End Of Line"<<endl;
				trim(data);

				if(data.length()>0){

					//cout<<"						--->	" <<data<<endl;

					x=0;							
				}

			}

    
			inFile.close();
		}

		else cout << "Unable to open file"; 
	}

	void processData(){
		vector<int> indexToRemFromData;
		vector<int> indexToRemFromHTML;

		string tagInTable;
		size_t found;

		map<int,string>::iterator it,next,current;
		
		map<string,string> reservedTagsType_0;		
		map<string,string> reservedTags_Emphasis;	
		

		reservedTagsType_0["<style"]="</style>";
		reservedTagsType_0["<script"]="</script>";
		reservedTagsType_0["<!--[if"]="<![endif]-->";
		

		reservedTags_Emphasis["<i>"]="</i>";
		reservedTags_Emphasis["<b>"]="</b>";
		reservedTags_Emphasis["<small>"]="</small>";
		reservedTags_Emphasis["<strong>"]="</strong>";

		int countX=0;
		for (next=htmlRecords.begin(), it=next++; next!=htmlRecords.end() && it!=htmlRecords.end(); it++,next++){
			//cout<<++countX<<endl;;
			
			
			//cout<<it->second<<"	"<<next->second<<endl;
			tagInTable=it->second;
			
			found = tagInTable.find_first_of(" >");
			current=next;

			if (found!=string::npos){
				tagInTable.erase(found);
				//cout<<tagInTable<<endl;
				if (reservedTagsType_0.find(tagInTable)!=reservedTagsType_0.end()){
					string currentEndTag = next->second;
					string requiredEndTag   = reservedTagsType_0[tagInTable];
					
					while (currentEndTag.compare(requiredEndTag) != 0){
						indexToRemFromHTML.push_back(current->first);
						currentEndTag=(++current)->second;
						

					}
					for ( int i =it->first+1; i<current->first;i++){
						indexToRemFromData.push_back(i);

					}

				}

			}					

		}

		for(size_t i=0; i<indexToRemFromData.size(); i++){
			//htmlRecords[indexToRemFromData[i]]=dataRecords[indexToRemFromData[i]];
			dataRecords.erase(indexToRemFromData[i]);
		}
		
		for(size_t i=0; i<indexToRemFromHTML.size(); i++){
			//htmlRecords[indexToRemFromData[i]]=dataRecords[indexToRemFromData[i]];
			htmlRecords.erase(indexToRemFromHTML[i]);
		}
	
	}
		



	void dataMine(){
		int indexDataTo=0, indexDataFrom=0;
		string currentHeader="";
		string documentTitle="";
		map<int,string>::iterator it,next,current,itData;

		vector <vector <string> >  newData;
		map<string,string> headlineTags;
		map<string,string> dataTags;
		string tagInTable;
		size_t found;
		int row=-1;
		string currentTag;
		string requiredEndTag;
		stack <string> nestedTags;



		dataTags["<table"]="</table>";
		dataTags["<ul"]="</ul>";
		dataTags["<ol"]="</ol>";



		headlineTags["<title"]="</title>";
		headlineTags["<h1"]="</h1>";
		headlineTags["<h2"]="</h2>";
		headlineTags["<h3"]="</h3>";
		
		itData=dataRecords.begin();
		int countX=0;
		for (next=htmlRecords.begin(), it=next++; next!=htmlRecords.end() && it!=htmlRecords.end(); it++,next++){
			countX++;

			if (countX>1223){
				x=0;
					//break;
			}


			//cout<<"InDataMine	"<<countX<<endl;
			//cout<<it->second<<"	"<<next->second<<endl;
			tagInTable=it->second;
			
			found = tagInTable.find_first_of(" >");
			current=next;

			if (found!=string::npos){
				tagInTable.erase(found);
				//cout<<tagInTable<<endl;	
				//xxxx-----------------------
				map<int,string>::iterator itFrom, itTo,itCurrent;
				int headDataFrom, headDataTo;
				if ( headlineTags.count(tagInTable)==1  ){
					requiredEndTag   = headlineTags[tagInTable];
					itCurrent=it;
					itFrom=itData;
					headDataFrom=itCurrent->first;
					
					while(itCurrent->second!=requiredEndTag){
						itCurrent++;
					}
					headDataTo=itCurrent->first;
					x=0;
					while(itFrom!=dataRecords.end() && itFrom->first < headDataTo){
						if( itFrom->first > headDataFrom  && itFrom->second.size()>0){
							//cout<<"______________"<<itFrom->second;
							currentHeader=itFrom->second;
							if (requiredEndTag == "</title>" ) {
								documentTitle=currentHeader;
							}
						}
						itFrom++;
						x=0;
					}
				}
				//---------------------------
				
				// found list or table
				if (dataTags.count(tagInTable)==1  ){
					
					currentTag = next->second;
					   
					requiredEndTag   = dataTags[tagInTable];

					//while list or table continue

					while(!nestedTags.empty()){
						nestedTags.pop();
					}
					//nestedTags.empty();
					
					
					nestedTags.push(requiredEndTag);

					//******
					newData.push_back( vector< string > (0,"") );
					row++;
					newData[row].push_back("****************	"+currentHeader+"	****************");	
					newData.push_back( vector< string > (0,"") );
					row++;
					//*******

					//while (currentTag.compare(requiredEndTag) != 0 ){  - old
					while (nestedTags.size()>0  ){
						size_t positionToCut = currentTag.find_first_of(" >");
						if (positionToCut !=string::npos){
							currentTag.erase(positionToCut);
						}

						//if ( currentTag.compare("<ul")==0 || currentTag.compare("<table")==0 || currentTag.compare("<ol")==0 )  {    //-old
						if ( dataTags.count (currentTag)==1 )  { 
							//newData.clear();
							nestedTags.push(dataTags[currentTag]);
							x=0;
						}

						if ( (currentTag.compare("<li")==0 || currentTag.compare("<tr")==0  ) && nestedTags.size()==1  ){
							//newData.push_back( vector< string > (0,"") );
							indexDataFrom=current->first;
							indexDataTo=0;
							//row++;
						}

						if ( currentTag.compare("</li")==0 || currentTag.compare("</tr")==0   ) {

/*							//----
							if (nestedTags.size()==1){
								indexDataFrom=current->first;
								indexDataTo=0;
							}
							//---
*/
							if (itData==dataRecords.end() ){
								break;
							}
							indexDataTo=current->first;
							vector <string> currentRow;
							bool firstPass= true;
							while(itData!=dataRecords.end() && itData->first < indexDataTo){
								if (itData->first < indexDataFrom){
									itData++;
									continue;
								}
								if (firstPass){
									newData.push_back( vector< string > (0,"") );
									row++;
								}
								firstPass=false;
								
								newData[row].push_back(itData->second);
								itData++;
							}

						}
						if (currentTag+'>'==nestedTags.top() ) {
						//if ( currentTag.compare("</ul")==0 || currentTag.compare("</table")==0 || currentTag.compare("</ol")==0  ){
							x=0;
							nestedTags.pop();
							x=0;
										newData.push_back( vector< string > (0,"") );
										row++;
						}

						currentTag=(++current)->second;


					}
					next=current;
					it=next++;
					//for (itData=datRecords.begin(); itData!=htmlRecords.end() ; it++){
					//	if (it->first>)
					//}
					
				}
			}
		}
		cout<<endl<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
		cout<<""<<documentTitle<<endl;
		cout<<endl<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
		for(int i=0;i<newData.size();i++){
			cout<<endl;//<<"--------------------------------------------------------------------------------"<<endl;
			if ( newData[i].size()==0){
			cout<<endl<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
			}
			for(int j=0;j<newData[i].size(); j++){
				cout<<setw( 40 )<<left<<newData[i][j];
			}


		}
		cout<<endl<<"--------------------------------------------------------------------------------"<<endl;

	}

		
};




int main(int argc, char* argv[]){
	string pathIn=PATH;
	if (argc ==2 ) {
		pathIn=argv[1];
		cout << pathIn;
	}

	
	Collect collect;
	//vector <int> v1,v2;
	//v1.assign(3,3) ; v1[1]=-1;v1[2]=4;
	//v2.assign(3,1); v2[1]=0;v2[2]=0;
	//vector <double >vv1=collect.normilizeVector( v1 );
	//vector <double >vv2=collect.normilizeVector( v2 );
	//double xx=collect.crossProduct(vv1,vv2);
	collect.getData(pathIn);
	collect.processData();
	//collect.frequencyAllignment();
	//collect.processVector();
	collect.dataMine();

	//cin.get();
		
return 0;}
