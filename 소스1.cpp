#include<iostream>
#include<stack>
#include<string>
#include <algorithm> 
#include<vector>

using namespace std;

class node {	
public:
	char input;
	int to;
	node* next;
};

bool accept = false;


int prec(char c) {
	if (c == '*') {
		return 3;
	}
	else if (c == '.') {
		return 2;
	}
	else if (c == '+') {
		return 1;
	}
	else {
		return -1;
	}
}



string post(string s)
{
	stack<char> st;
	st.push('N');
	int l = s.length();
	string ns;
	for (int i = 0; i < l; i++)
	{
		if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z')) {
			ns += s[i];
		}
		else if (s[i] == '(') {
			st.push('(');
		}
		else if (s[i] == ')')
		{
			while (st.top() != 'N' && st.top() != '('){
				char c = st.top();
				st.pop();
				ns += c;
			}
			if (st.top() == '('){
				char c = st.top();
				st.pop();
			}
		}
		else {
			while (st.top() != 'N' && prec(s[i]) <= prec(st.top())){
				char c = st.top();
				st.pop();
				ns += c;
			}
			st.push(s[i]);
		}
	}
	while (st.top() != 'N'){
		char c = st.top();
		st.pop();
		ns += c;
	}
	return ns;
}

bool ischar(char c) {		//alphabet인지 아닌지 확인하는 함수
	if ((c >= 'a' && c <= 'z') || (c >= 'a' && c <= 'z'))return true;
	else return false;	
}

string convert(string input) {			//regular expression에 concatenation이 생략된 경우 .을 추가하는 함수
	string result;
	for (int i = 0; i < input.length()-1; i++) {
		if (input[i] == '|') {
			result.push_back('+');
		}
		else if (ischar(input[i]) && input[i + 1] == '(') {
			result.push_back(input[i]);
			result.push_back('.');
		}
		else if (ischar(input[i]) && ischar(input[i + 1])) {
			result.push_back(input[i]);
			result.push_back('.');
		}
		else if (input[i] == ')' && ischar(input[i + 1])) {
			result.push_back(input[i]);
			result.push_back('.');
		}
		else if (input[i] == '*' && ischar(input[i + 1])) {
			result.push_back(input[i]);
			result.push_back('.');
		}
		else result.push_back(input[i]);
	}
	result.push_back(input[input.length()-1]);
	
	return result;
}

void printnode(vector<node*> v) {			//NFA 결과를 출력해주는 함수
	cout << "___________________________________________" << endl;
	cout << "| state\t| input/next states" << endl;
	cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;


	for (int i = 0; i < v.size(); i++) {
		cout << "| " << i << "\t|";
		node* head = v[i];
		while (head != NULL) {
			if (head->to < 0) {
				head = head->next;
				continue;
			}
			cout << head->input <<"/" << head->to<<"    ";
			head = head->next;
		}
		cout << endl;
	}
	cout << "___________________________________________" << endl;
}

node* makenode(char in) {
	node* a = new node;
	a->input = in;
	a->to = -1;
	a->next = NULL;
	return a;
}

node* copynode(node* a) {
	node* b = new node;
	b->input = '@';
	b->to = -1;
	b->next = NULL;
	return b;
}


void andd(vector<node*>& v, vector<vector<int> >& st) {		//concatenation 역할을 수행하는 함수
	int x, y;
	int first, last1;
	y = st[st.size() - 1][0];
	x = st[st.size() - 2][1];
	first = st[st.size() - 2][0];
	last1 = st[st.size() - 1][1];

	st.pop_back();
	st.pop_back();

	vector<int> ptemp;
	ptemp.push_back(first);
	ptemp.push_back(last1);
	st.push_back(ptemp);

	node* last = v[y];
	node* lnode = v[x];
	node* temp = copynode(last);

	while (lnode->next != NULL) {
		lnode = lnode->next;
	}
	lnode->next = temp;
	lnode->to = y;
}

void anddfinal(vector<node*>& v, vector<vector<int> >& st) {
	//마지막 state에 transition이 loop 형태로 꼬여있는 것을 푸는 함수
	int endindex = st[st.size() - 1][1];
	node* endstate = v[endindex];
	if (endstate->input != '@') {
		node* finalstate = makenode('@');
		v.push_back(finalstate);
		int finalindex = v.size() - 1;

		endstate->to = finalindex;
		
		st[st.size() - 1][1] = finalindex;
	}
}

void orr(vector<node*>& v, vector<vector<int> >& st) {
	int x, y, x1, y1;
	x = st[st.size() - 2][0];
	y = st[st.size() - 1][0];
	x1 = st[st.size() - 2][1];
	y1 = st[st.size() - 1][1];
	node* start = makenode('@');
	node* end = makenode('@');
	v.push_back(start);
	int firstnode = v.size() - 1;
	v.push_back(end);
	int endnode = v.size() - 1;

	st.pop_back();
	st.pop_back();

	vector<int> ptemp;
	ptemp.push_back(firstnode);
	ptemp.push_back(endnode);
	st.push_back(ptemp);

	for (int i = 0; i < v.size() - 2; i++) {	
	//기존 x와 y에 해당하는 state에 transition하는 것들을 새로운 state로 향하게 수정
		node* h = v[i];
		while (h->next != NULL) {
			if (h->to == x || h->to == y) {
				h->to = firstnode;
			}
			h = h->next;
		}
	}

	node* temp = copynode(v[x]);
	node* temp1 = copynode(v[y]);

	//alternative가 시작되는 state가 갈라지는 두 state를 가리키게 수정
	node* t = v[firstnode];
	while (t->next != NULL) {
		t = t->next;
	}
	t->to = x;
	t->next = temp;
	t->next->to = y;
	t->next->next = temp1;

	//갈라진 state의 끝이 하나의 state로 모이게 수정
	node* adlink = v[x1];
	while (adlink->next != NULL) {
		adlink = adlink->next;
	}
	adlink->to = endnode;
	adlink->next = copynode(end);

	node* adlink1 = v[y1];
	while (adlink1->next != NULL) {
		adlink1 = adlink1->next;
	}
	adlink1->to = endnode;
	adlink1->next = copynode(end);
}


void closure(vector<node*>& v, vector<vector<int> >& st) {
	int x, x1;
	x = st[st.size() - 1][0];
	x1 = st[st.size() - 1][1];
	node* s = makenode('@');
	v.push_back(s);
	int firstnode = v.size() - 1;
	st.pop_back();
	vector<int> ptemp;
	ptemp.push_back(x);
	ptemp.push_back(firstnode);
	st.push_back(ptemp);

	for (int i = 0; i < v.size() - 2; i++) {
		node* h = v[i];
		while (h->next != NULL) {
			if (h->to == x) {
				h->to = firstnode;
			}
			h = h->next;
		}
	}

	node* t = v[x1];
	while (t->next != NULL) {
		t = t->next;
	}
	t->to = x;
	t->next = copynode(t);
	t->next->to = firstnode;
	t->next->next = copynode(s);

	node* tmp = v[x];
	while (tmp->next != NULL) {
		tmp = tmp->next;
	}
	tmp->to = firstnode;
	tmp->input = '@';
	tmp->next = NULL;
}

void isaccepted(vector<node*>& v, int startindex, int finalindex, string input, int i, int count =0) {
	node* state = v[startindex];

	if (count > v.size())return;

	if (i > input.length()-1 && i <= input.length())  {
		if (startindex == finalindex) {
			accept = true;
			return;
		}
	}
	
	while (state != NULL) {
		if (state->to < 0) {
			state = state->next;
			continue;
		}
		if (state->input == input[i]) {
			isaccepted(v, state->to, finalindex, input, i + 1);
		}
		else if (state->input == '@' ) {
			isaccepted(v, state->to, finalindex, input, i, count+1);
		}
		state = state->next;
	}

}


int main() {
	string in;
	cout << "Enter a regular expression\n";
	cin >> in;
	in = convert(in);
	cout << "\nConverted a regular expression: "<<in<<endl;

	string o;
	vector<node*> v;
	o = post(in);
	cout << "\npostfix expression is " << o << endl;
	vector<vector<int>> st;
	int firstnode = 0;
	for (int l = 0; l < o.length(); l++) {
		if (o[l] != '+' && o[l] != '*' && o[l] != '.') {
			node* temp = makenode(o[l]);
			v.push_back(temp);
			vector<int> ptemp;
			ptemp.push_back(v.size() - 1);
			ptemp.push_back(v.size() - 1);
			st.push_back(ptemp);
		}
		else if (o[l] == '.') {
			andd(v, st);
		}
		else if (o[l] == '+') {
			orr(v, st);
		}
		else if (o[l] == '*') {
			closure(v, st);
		}
	}
	anddfinal(v, st);
	cout << "\ntrainsition table for given regular expression is - \n";
	printnode(v);
	cout << endl;
	cout << "starting state is ";
	cout << st[st.size() - 1][0] << endl;
	cout << "ending state is ";
	cout << st[st.size() - 1][1] << endl;

	string inputstring;
	while (true) {
		if (inputstring == "-1")return 0;
		accept = false;
		cout << "\nEnter a string: \n";
		cin >> inputstring;
		isaccepted(v, st[st.size() - 1][0], st[st.size() - 1][1], inputstring, 0);
		if (accept)cout << "accepted\n";
		else cout << "nonaccepted\n";
	}
		
	return 0;
}
