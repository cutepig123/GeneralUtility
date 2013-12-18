#include <stdio.h>
#include <vector>
#include <string>

class Trie
{
public:	
	void InsertStr(const char *s)
	{
		int N=strlen(s);
		for(int nSub=1; nSub<=N;nSub++)
		{
			for(int i=0;i<=N-nSub;i++)
			{
				InsertStr( nSub, &s[i] );
			}
		}
	}
	
	void Dump() const
	{
		std::vector<char> s;
		Dump_recur(root_,s);
	}
	
	void Dump2() const
	{
		Dump_recur2(root_,0);
	}
private:

	
	
	void InsertStr(int n, const char *s)
	{
		printf("InsertStr ");
		for(int i=0; i<n;i++)
			printf("%c", s[i]);
		printf("\n");
		
		Node *pN = &root_;;
		for(int i=0; i<n;i++)
		{
			//srch s[i] in aChild[]
			bool bFound = false;
			for(int j=0,nc=pN->aChild.size(); j<nc; j++)
			{
				if( pN->aChild[j].value== s[i] )
				{
					pN = &pN->aChild[j];
					bFound = true;
					break;
				}
			}
			
			//
			if(!bFound)
			{
				pN->aChild.push_back(Node(s[i]));
				pN = &pN->aChild.back();
			}
		}
	}
	
	struct Node
	{
		int value;
		std::vector<Node> aChild;
		
		Node()
		:value(0)
		{}
		
		explicit Node(int v)
		:value(v)
		{}
	};
	
	void Dump_recur2(const Node &node, int h) const
	{
		printf("Level %d:", h);
		
		int i=0,n=node.aChild.size();
		for(; i<n; i++)
		{
			printf("%c", node.aChild[i].value );
		}
		
		for(; i<n; i++)
		{
			Dump_recur2( node.aChild[i], h+1 );
		}
	}
	
	void Dump_recur(const Node &node, std::vector<char> &s) const
	{
		int i=0,n=node.aChild.size();
		if(n==0)
		{
			for(int i=0; i<s.size();i++)
				printf("%c", s[i]);
			printf("\n");
			return;
		}
			
		for(; i<n; i++)
		{
			s.push_back( node.aChild[i].value );
			Dump_recur( node.aChild[i], s );
			s.pop_back();
		}
	}
	
	Node root_;
};

void main()
{
	Trie t;
	t.InsertStr("Hello");
	
	printf("Dump....\n");
	t.Dump();
	
	printf("Dump2....\n");
	t.Dump2();
}
