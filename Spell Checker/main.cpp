//
//  main.cpp
//  Spell Checker
//  Demonstrates a simplistic implementation of the Levenstein Distance algorithm
//    in comparing strings
//
//  Created by Clara Fairbanks on 11/14/18.
//  Copyright © 2018 Clara Fairbanks. All rights reserved.
//

#include <stdio.h>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <unordered_set>

using namespace std;

unordered_set<string> getDict( string dictFile ){
    
    ifstream inFile;
    inFile.open( dictFile );
    while ( !inFile ) {
        cout << "Unable to open file dictFile.txt\n";
        cout<< "Please input the dictionary you would like to use : "<<endl;
        cin >> dictFile;
    }
    
    string x;
    unordered_set<string> dictionary;
    while ( inFile >> x ) {
        dictionary.insert( x );
    }
    
    return dictionary;
}

vector<string> getInput( string inputFile ){
    
    ifstream inFile;
    inFile.open( inputFile );
    while ( !inFile ) {
        cout << "Unable to open file inputFile.txt\n";
        cout<< "Please input the text file you would like to check : "<<endl;
        cin >> inputFile;
    }
    
    string x;
    vector<string> text;
    while ( inFile >> x ) {
        
        for( size_t i = 0; i <+ x.length(); ++i ){
            
            x[i] = tolower( x[i] );
            if( ispunct( x[i] ) )
            x.erase( i--, 1 );
        }
        
        text.push_back( x );
    }
    return text;
}

size_t levenshtein( string a, string b ){
    
    //ensure the larger word is the second string
    if( b.size() < a.size() )
    swap( a, b ) ;
    
    //create two vectors for comparison
    vector<size_t> current( b.size()+1 ) ;
    vector<size_t> previous( current ) ;
    
    //iterate through each character
    iota( previous.begin(), previous.end(), 0 ) ;
    
    //compare each character in the first string to each in the second string
    for( size_t i = 0 ; i < a.size() ; ++i ){
        
        current[0] = i+1 ;
        for( size_t j = 0 ; j < b.size() ; ++j ){
            
            current[j+1] = min( min( current[j], previous[j+1] ) + 1,
                               previous[j] + ( a[i] != b[j] ) ) ;
        }
        current.swap(previous);
    }
    return previous.back() ;
}

vector<string> find_similar( string word, unordered_set<string> dict ){
    
    //receive a word that is NOT found in the dict
    //create a vector to contain potential matches
    vector<string> similar[2] ;
    
    //for each existing word in the dictionary, check if it is a match
    for( const string equal : dict ){
        
        //calculate LD between input and dictionary items
        size_t dist = levenshtein( word, equal );
        
        //the dict item will be a pushed as a match if the LD is one
        if( dist < 2 )
        similar[dist].push_back(equal);
    }
    
    //for each word in nearest, if it exists, return it
    for( const auto& equalWord : similar ){
        
        if( !equalWord.empty() )
        return equalWord;
    }
    
    return {};
}

void check_spelling( const vector<string> text, const unordered_set<string>& dict ){
    
    ofstream results;
    results.open ("spell_checker_results.txt");
    if( results.fail() ){
        cout<<"no result file \n";
    }
    
    for( const string& word : text ){
        
        if( dict.find( word ) != dict.end() )
        continue;
        else{
            results << word << " ---> " ;
            for( const auto& nearest : find_similar( word, dict ) )
            results << nearest << ' ' ;
            results << '\n' ;
        }
    }
    results.close();
}


int main(){
    
    string dictionary, input;
    
    cout<< "Please input the dictionary you would like to use : "<<endl;
    cin >> dictionary;
    const unordered_set<string>& dict = getDict( dictionary );
    
    cout<< "Please input the text you would like spellcheck : "<<endl;
    cin >> input;
    const vector<string> text = getInput( input );
    
    auto start_spell_check = chrono::high_resolution_clock::now();
    
    check_spelling( text, dict );

    auto finish_spell_check = chrono::high_resolution_clock::now();
    
    chrono::duration<double> elapsed_time = finish_spell_check - start_spell_check;
    
    cout<<"/nCompleted spell check, please open 'spell_checker_results.txt' to see the results\n";
    cout <<"\ntotal time: "<< elapsed_time.count() <<endl;
    
    return 0;
}
