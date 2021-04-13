#include<bits/stdc++.h>
#include "lexer.cpp"
#define watch(x) cerr << "\n" << (#x) << " = " << (x) << "\n"
using namespace std;

map<string, int> terminal_index;
map<string, int> non_terminal_index;
vector<vector<string>> action(200, vector<string>(200, ""));
vector<vector<string>> Goto(200, vector<string>(200, ""));
vector<pair<string, vector<string>>> grammar(100, make_pair(" ", vector<string>(0, " ")));
bool is_parsable = false;
bool has_error = false;

vector<int> reduction_steps;

int grammar_length = 82;
int terminal_count = 35;  // Number of columns in action table
int non_terminal_count = 36; // Number of columns in goto table
int rows = 152;

void parsing(vector<pair<string, string>> input, vector<int> token_line){
	stack<string> token_stack;
	token_stack.push("0");

	int i = 0;
	while(i < input.size()){
		string curr_input = input[i].second;   
		int line_number = token_line[i];     
   
		int token_index = terminal_index[curr_input];   
		int stack_top = 0; 

		stack_top = stoi(token_stack.top());  
		        
		string curr_action = action[stack_top][token_index]; 

		if(curr_action[0] == 's'){
			string shift_num = curr_action.substr(1);
			token_stack.push(curr_input);
			token_stack.push(shift_num);
		}
		else if(curr_action[0] == 'r'){
			int reduce_num = stoi(curr_action.substr(1));
			reduction_steps.push_back(reduce_num);
			int rhs_size = grammar[reduce_num].second.size();
			string lhs = grammar[reduce_num].first;

			int j = 0;
			while(j < (2 * rhs_size)){
				token_stack.pop();
				j++;
			}
			
			int curr_stack_top = 0;
			curr_stack_top = stoi(token_stack.top());
			token_stack.push(lhs);
			
			int _non_terminal_index = non_terminal_index[lhs];
			string curr_goto = Goto[curr_stack_top][_non_terminal_index];
			if(curr_goto == "error"){
				cout << "Error on line number " << line_number - 1 << "\n";
				return;
			}
			else{
				token_stack.push(curr_goto);
			}
			i--;
		}
		else if(curr_action == "acc"){
			is_parsable = true;
			return;
		}
		else{
			has_error = true;
			cout << "Error on line number " << line_number - 1 << "\n";
			int curr_line_num = line_number;
			token_stack.top() = "0";
			while((token_line[i] == curr_line_num) && i < input.size()){
				i++;
			}
			i--;
		}
		i++;
	}
}


void read_action_table(){
	freopen("parse_table.txt", "r", stdin);

	int i = 0;
	string terminal_input;
	while(i < terminal_count){
		cin >> terminal_input;
		terminal_index[terminal_input] = i - 1;
		i++;
	}

	string cell_input;
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < terminal_count; j++){
			cin >> cell_input;
			if(j == 0)
				continue;
			action[i][j - 1] = cell_input;
		}
	}	
}


void read_goto_table(){
	freopen("goto.txt", "r", stdin);

	int i = 0;
	string non_terminal_input;
	while(i < non_terminal_count){
		cin >> non_terminal_input;
		non_terminal_index[non_terminal_input] = i - 1;
		i++;
	}

	string cell_input;
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < non_terminal_count; j++){
			cin >> cell_input;
			if(j == 0)
				continue;
			Goto[i][j - 1] = cell_input;
		}
	}	
}

void read_grammar(){

	freopen("grammar.txt", "r", stdin);
	
	int i = 0;
	string token;
	bool l = true, r = false;
	while(i < grammar_length){
		cin >> token;
		if(token == "end"){
			i++;
			l = true;
			r = false;
			continue;
		}
		if(token == "->"){
			l = false;
			r = true;
			continue;
		}
		if(l){
			grammar[i].first = token;
		}
		if(r){
			if(token != "''")
				grammar[i].second.push_back(token);
		}
	}	
}

void lexer(){
	freopen("input.txt", "r", stdin);
	while(getline(cin, line)){
		parse(line);
		line_num++;
	}
}

void create_parse_tree(){
	vector<string> current_row;

	current_row.push_back(grammar[reduction_steps[0]].first);

	for(int i = 0; i < reduction_steps.size(); ++i){
		for(auto it: current_row){
			cout << it << " ";
		}

		cout << "\t\t";
		cout << "[ ";
		cout << grammar[reduction_steps[i]].first << " -> ";
		for(auto it: grammar[reduction_steps[i]].second){
			cout << it << " ";
		} 
		cout << "]";
		
		cout << "\n";
		int j = current_row.size() - 1;
		while(j >= 0){
			if(current_row[j] == grammar[reduction_steps[i]].first){
				break;
			}
			j--;
		}
		j = max(0, j);
		current_row.erase(current_row.begin() + j);
		for(auto it: grammar[reduction_steps[i]].second){
			current_row.insert(current_row.begin() + j, it);
			j++;
		}
	}
}

int main(){
	ios_base::sync_with_stdio(0);
    cin.tie(0); 
    cout.tie(0);

	freopen("output.txt", "w", stdout);

	read_action_table();

	read_goto_table();

	read_grammar();
		
	lexer();

	is_parsable = false;

	token_list.push_back(make_pair("$", "$")); // Appending $ to the end of input string
	token_line.push_back(token_line[token_line.size() - 1]);

	parsing(token_list, token_line);

	if(is_parsable && !has_error){
		cout << "\nThe given input is LALR(1) Parsable\n\n\n";
	}
	else{
		if(has_error){
			cout << "\nThere is an error\n\n\n";
		}
		else
			cout << "\nThe given input is not LALR(1) Parsable\n\n\n";
	}

	reverse(reduction_steps.begin(), reduction_steps.end());

	create_parse_tree();

	return 0;
}