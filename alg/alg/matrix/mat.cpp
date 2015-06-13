#include "mat.h"
#include "assert.h"

/////////////IO/////////////////////////
void MatPrint(Matrix const &l)
{
	for(int y=0; y<l.sz.y; y++)
	{
		for(int x=0;x<l.sz.x;x++)
		{
			printf("%5g ", GETELEM(l,y,x));
		}
		printf("\n");
	}
}

bool MatEqu(Matrix const &l, Matrix const &r)
{
	if(!(l.sz==r.sz))
		return false;

	for(int y=0; y<l.sz.y; y++)
	{
		for(int x=0;x<l.sz.x;x++)
		{
			if( fabs( GETELEM(l,y,x)- GETELEM(r,y,x) )>0.001 )
				return false;
		}
	}

	return true;
}

/////////////OP(0)///////////////////////
template <class OP>
int MatOp0(Matrix *d, OP op)
{
	int wSts = 0;

	for(int y=0; y<d->sz.y; y++)
	{
		for(int x=0;x<d->sz.x;x++)
		{
			op(y,x,&GETELEM(*d,y,x));
		}
	}
//Exit:
	return wSts;
}

void MatOnes(Matrix *d)
{
	MatOp0(d, [](int y,int x, double *v){*v= 1.0;});
}

void MatZeros(Matrix *d)
{
	MatOp0(d, [](int y,int x, double *v){*v= 0.0;});
}

void MatIdentity(Matrix *d)
{
	MatOp0(d, [](int y,int x, double *v){*v= (x==y)?1.0:0.0;});
}

void MatAbs(Matrix *d)
{
	MatOp0(d, [](int y,int x, double *v){*v= fabs(*v);});
}

void MatSet(Matrix *d, double val)
{
	MatOp0(d, [val](int y,int x, double *v){*v= val;});
}

/////////////OP(1)///////////////////////
template <class OP>
int MatOp1(Matrix const &l, Matrix *d, OP op)
{
	int wSts = 0;

	if( !(l.sz == d->sz) )
	{
		wSts = -1;
		CHK_ERR;
	}

	for(int y=0; y<l.sz.y; y++)
	{
		for(int x=0;x<l.sz.x;x++)
		{
			GETELEM(*d,y,x) = op(GETELEM(l,y,x));
		}
	}
Exit:
	return wSts;
}

int MatAddC(Matrix const &l, double c, Matrix *d)
{
	return MatOp1(l,d,[c](double l){return l+c;});
}

int MatSubC(Matrix const &l, double c, Matrix *d)
{
	return MatOp1(l,d,[c](double l){return l-c;});
}

int MatMulC(Matrix const &l, double c, Matrix *d)
{
	return MatOp1(l,d,[c](double l){return l*c;});
}
int MatDivC(Matrix const &l, double c, Matrix *d)
{
	return MatOp1(l,d,[c](double l){return l/c;});
}
/////////////OP(2)///////////////////////
template <class OP>
int MatOp2(Matrix const &l, Matrix const &r, Matrix *d, OP op)
{
	int wSts = 0;

	if( !(l.sz == r.sz) || !(l.sz == d->sz) )
	{
		wSts = -1;
		CHK_ERR;
	}

	for(int y=0; y<l.sz.y; y++)
	{
		for(int x=0;x<l.sz.x;x++)
		{
			GETELEM(*d,y,x) = op(GETELEM(l,y,x), GETELEM(r,y,x));
		}
	}
Exit:
	return wSts;
}

int MatAdd(Matrix const &l, Matrix const &r, Matrix *d)
{
	return MatOp2(l,r,d,[](double l,double r){return l+r;});
}

int MatSub(Matrix const &l, Matrix const &r, Matrix *d)
{
	return MatOp2(l,r,d,[](double l,double r){return l-r;});
}

int MatDotMul(Matrix const &l, Matrix const &r, Matrix *d)
{
	return MatOp2(l,r,d,[](double l,double r){return l*r;});
}
int MatDotDiv(Matrix const &l, Matrix const &r, Matrix *d)
{
	return MatOp2(l,r,d,[](double l,double r){return l/r;});
}
int MatMul(Matrix const &l, Matrix const &r, Matrix *d)
{
	int wSts = 0;

	if( !(l.sz.x == r.sz.y) || !(l.sz.y == d->sz.y) || !(r.sz.x == d->sz.x) )
	{
		wSts = -1;
		CHK_ERR;
	}

	for(int y=0; y<d->sz.y; y++)
	{
		for(int x=0;x<d->sz.x;x++)
		{
			double v=0.0f;
			for( int k=0; k<l.sz.x; k++)
				v += GETELEM(l,y,k) * GETELEM(r,k,x);
			GETELEM(*d,y,x) = v;
		}
	}
Exit:
	return wSts;
}

class CMatrix
{
public:
	CMatrix(int y,int x,double const a[]=0)
	{
		m_.sz.x = m_.step=x;
		m_.sz.y= y;

		if( x || y )
		{
			p_.resize(y*x);
			m_.ptr = &p_[0];
			if(a)
				std::copy(a, a+x*y, p_.begin());
		}
		else
			m_.ptr = NULL;
	}

	CMatrix(CMatrix const &l)
		:m_(l.m_), p_(l.p_)
	{
		if(m_.ptr)
			m_.ptr = &p_[0];
	}

	CMatrix& operator=(CMatrix const &l)
	{
		swap( CMatrix(l) );
		return *this;
	}
	CMatrix& operator+=(CMatrix const &l)
	{
		if( !(l.get().sz==get().sz) )
			throw std::invalid_argument("inv dim");

		MatAdd(get(),l.get(),&get());
		return *this;
	}
	CMatrix& operator+=(double c)
	{
		MatAddC(get(),c,&get());
		return *this;
	}
	CMatrix& operator-=(CMatrix const &l)
	{
		if( !(l.get().sz==get().sz) )
			throw std::invalid_argument("inv dim");

		MatSub(get(),l.get(),&get());
		return *this;
	}
	CMatrix& operator-=(double c)
	{
		MatSubC(get(),c,&get());
		return *this;
	}
	CMatrix& operator*=(CMatrix const &l)
	{
		if( !(l.get().sz.y==get().sz.x) )
			throw std::invalid_argument("inv dim");

		MatMul(get(),l.get(),&get());
		return *this;
	}
	CMatrix& operator*=(double c)
	{
		MatMulC(get(),c,&get());
		return *this;
	}
	CMatrix& operator/=(double c)
	{
		MatDivC(get(),c,&get());
		return *this;
	}
	void swap(CMatrix &l)
	{
		using namespace std;
		std::swap(m_, l.m_);
		std::swap(p_, l.p_);
	}

	const Matrix& get() const
	{
		return m_;
	}

	Matrix& get()
	{
		return m_;
	}
private:
	Matrix m_;
	std::vector<double> p_;
};

bool operator==(CMatrix const &l,CMatrix const &r)
{
	return MatEqu(l.get(),r.get() );
}
void Ones(CMatrix *d)
{
	MatOnes(&d->get());
}

void Zeros(CMatrix *d)
{
	MatZeros(&d->get());
}

void Identity(CMatrix *d)
{
	MatIdentity(&d->get());
}
void Abs(CMatrix *d)
{
	MatAbs(&d->get());
}
void Set(CMatrix *d, double val)
{
	MatSet(&d->get(),val);
}
CMatrix operator+(CMatrix const &l, double c)
{
	CMatrix r(l);
	return r += c;
}
CMatrix operator+(double c, CMatrix const &l)
{
	CMatrix r(l);
	return r += c;
}
CMatrix operator+(CMatrix const &l, CMatrix const &r)
{
	CMatrix res(l);
	return res += r;
}

CMatrix operator-(CMatrix const &l,double c)
{
	CMatrix res(l);
	return res -= c;
}

CMatrix operator-(CMatrix const &l, CMatrix const &r)
{
	CMatrix res(l);
	return res -= r;
}

CMatrix operator*(double c,CMatrix const &l)
{
	CMatrix res(l);
	return res *= c;
}

CMatrix operator*(CMatrix const &l,double c)
{
	CMatrix res(l);
	return res *= c;
}
CMatrix operator/(CMatrix const &l,double c)
{
	CMatrix res(l);
	return res /= c;
}

CMatrix Indentity(int n)
{
	CMatrix m(n,n);
	Identity(m);
	return m;
}

void Print(CMatrix const &l)
{
	MatPrint(l.get());
}

int main()
{
	CMatrix a(3,3),b(3,3),c(3,3),d(3,3);

	Ones(&a);
	Ones(&b);

	c = a*2;
	Set(&d,2);
	assert(c==d);

	Set(&d,2);
	c=a+b;
	assert(c==d);

	double ae[]={3,5,1,1};
	CMatrix e(4,1,ae);
	//Print(*c.get());
}
