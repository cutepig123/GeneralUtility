//#include "stdafx.h"
#include"VGACard.h"
#include"Screen.h"
const int CursorTypeAddr = 0x460;//光标类型存储位置，字大小
const int CursorPositionAddr = 0x450;//光标位置存储地址，8个字大小
const int charWidth = 10;
const int charHeight = 16;
void VGACard::init()
{
	this->currentPage = 0;
	this->textMemAddr = this->memStartAddr + 0xb8000;
	this->cursor[0].y = 0x5;
	this->cursor[0].cursor_top = 0x06;
	this->cursor[0].cursor_bottom = 0x07;
	this->charPerRow = 80;
	this->charPerCol = 25;
	this->pscreen->setScreen_Widht_Height(charWidth, charHeight, true);//缺省状态时状态3
	vgaMode = 0x3;//缺省状态时状态3
	this->vgaDataID = 0;//用于判断VGA数据是否已经跟新，如果跟新了就让screen重画
	memset(textMemAddr, 0, 1024*4); //把显存内存全部清零，有四页
	this->isReady = true;
	

}

void VGACard::linkScreen(Screen* pscreen)
{
	this->pscreen = pscreen;
	pscreen->pvga = this;
}
vector<int> VGACard::get_INT_shouldHandle()
{
	
    vector<int> INTarray;
	INTarray.push_back(0x10);//VGA需要处理INT 为10 的中断
	return INTarray;
	
}
void VGACard::HandleINT(U1 INT_num)
{
	U2 tmpCX = pCPU->cx;
	//INT_num 暂时用不到
	switch(pCPU->ah)
	{
	case 0:
		this->setDispalyMode();
		break;
	case 1:
		this->setCursor_type();
		break;
	case 2:
		this->setCursor_position();
		break;
	case 3:
		this->getCursor_position();
		break;
	case 4://获取光笔。。没有光笔
		break;
	case 5:
		this->setDisplayPage();
		break;
	case 6:
		this->screenUpScroller();
		break;
	case 7:
		this->screenDownScroller();
		break;
	case 8:
		this->getCharWithProperty();
		break;
	case 9:
		this->displayCharWithProperty();
		break;
	case 0xe:
		pCPU->cx = 1;
		this->displayCharWithProperty();
		pCPU->cx = tmpCX;
		break;
	case 0xa:
		this->displayChar();
		break;
	case 0xb:
		this->setPalette();
		break;
	case 0xc:
		this->displayPixel();
		break;
	case 0xd:
		this->getPixel();
		break;
	case 0xf:
		this->getVGAStatus();
		break;
	case 0x11:
		this->characterGenerate();
		break;
	case 0x12:
		if(0x10 == pCPU->bl)//获取VGA配置的子功能
			this->getVGAInfo();
		break;
	case 0x13:
		this->displayString();
		break;
	case 0x1a:
		if(pCPU->al ==0)
		{
			pCPU->al = 0x1a;//表示支持子功能
			pCPU->bh = 0x0;//带有模拟彩色VGA显示器,活动显示器代码
			pCPU->bl = 0x8;//可替换显示器代码
		}
		break;
	}
	this->vgaDataID++;//表示Screen需要重画
}


void VGACard::setDispalyMode()
{
	switch(pCPU->al)
	{
	case 0:
		this->charPerRow = 40;
		this->charPerCol = 25;
		this->pscreen->setScreen_Widht_Height(charWidth, charHeight, false);
		break;
	case 1:
		this->charPerRow = 40;
		this->charPerCol = 25;
		this->pscreen->setScreen_Widht_Height(charWidth, charHeight, true);
		break;
	case 2:
		this->charPerRow = 80;
		this->charPerCol = 25;
		this->pscreen->setScreen_Widht_Height(charWidth, charHeight, false);
		break;
	case 3:
		this->charPerRow = 80;
		this->charPerCol = 25;
		this->pscreen->setScreen_Widht_Height(charWidth, charHeight, true);
		break;
	case 7:
		this->charPerRow = 80;
		this->charPerCol = 25;
		this->pscreen->setScreen_Widht_Height(charWidth, charHeight, false);
		break;
	default:
		this->buildLog(Hardware::ErrorType::UnHandle,"setDispalyMode Unhandle the Display mode");
		this->showMessage("setDispalyMode Unhandle the Display mode");
		assert(false);//was not finished
		return;
	}
	vgaMode=pCPU->al;
	//如果第七位是0，会清空屏幕，因为这里是文本模式，第七位都是0，所以必清
	memset((void*)(textMemAddr+(currentPage*charPerRow*charPerCol)*2),0,charPerRow*charPerCol*2);//清屏
}
void VGACard::setCursor_type()
{
	bool isVisual = !(MASK_00001000 & pCPU->ch);
	for(int i=0; i<MAX_PAGE; i++)
	{
		this->cursor[i].cursor_top = pCPU->ch&MASK_00001111;
		this->cursor[i].cursor_bottom = pCPU->cl&MASK_00001111;
		this->cursor[i].isVisual = isVisual;
	}
	*(U2*)(memStartAddr + CursorTypeAddr) = pCPU->cx;
}
void VGACard::setCursor_position()
{
	if(pCPU->bh < MAX_PAGE)
	{
		this->cursor[pCPU->bh].x = pCPU->dl;
		this->cursor[pCPU->bh].y = pCPU->dh;
		*(U2*)(memStartAddr + CursorPositionAddr + pCPU->bh*2/*更新bh页光标位置，因为是字单位，所以乘2*/) = pCPU->dx;
	}
}
void VGACard::setDisplayPage()
{
	this->currentPage = pCPU->al;
}
void VGACard::screenUpScroller()
{
	if(0==pCPU->al)
	{//清屏
		this->clsScreen();
		return;
	}
	U1 ch=pCPU->ch, cl=pCPU->cl, dh=pCPU->dh, dl=pCPU->dl;
	U1 scrollerNum = pCPU->al;//卷多少行
	for(int i=0; i<dh - ch - scrollerNum+1; i++)
	{//上卷
		memcpy((void*)(textMemAddr+(currentPage*charPerRow*charPerCol + (ch+i)*charPerRow+cl)*2),
			(void*)(textMemAddr+(currentPage*charPerRow*charPerCol + (ch+i+scrollerNum)*charPerRow+cl)*2),
			(dl - cl +1 )*2);
	}
	for(int i=0; i<dh - ch - scrollerNum+1; i++)
	{//填充空白行
		for(int j=0; j<dl-cl+1; j++)
		{
			*(textMemAddr+(currentPage*charPerRow*charPerCol + (ch+i+scrollerNum)*charPerRow+cl)*2)= ' ';
			*(textMemAddr+(currentPage*charPerRow*charPerCol + (ch+i+scrollerNum)*charPerRow+cl)*2+1)=pCPU->bh;//bh决定了空白行的属性
		}
	}
	
}
void VGACard::screenDownScroller()
{
	if(0==pCPU->al)
	{//清屏
		this->clsScreen();
		return;
	}
	U1 ch=pCPU->ch, cl=pCPU->cl, dh=pCPU->dh, dl=pCPU->dl;
	U1 scrollerNum = pCPU->al;//卷多少行
	for(int i=0; i<dh - ch - scrollerNum+1; i++)
	{//下卷
		memcpy((void*)(textMemAddr+(currentPage*charPerRow*charPerCol + (ch+i+scrollerNum)*charPerRow+cl)*2),
			(void*)(textMemAddr+(currentPage*charPerRow*charPerCol + (ch+i)*charPerRow+cl)*2),
			(dl - cl +1 )*2);
	}
	for(int i=0; i<dh - ch - scrollerNum+1; i++)
	{//填充空白行
		for(int j=0; j<dl-cl+1; j++)
		{
			*(textMemAddr+(currentPage*charPerRow*charPerCol + (ch+i)*charPerRow+cl)*2)= ' ';
			*(textMemAddr+(currentPage*charPerRow*charPerCol + (ch+i)*charPerRow+cl)*2+1)=pCPU->bh;//bh决定了空白行的属性
		}
	}
}
void VGACard::clsScreen()
{
	memset((void*)(textMemAddr+(currentPage*charPerRow*charPerCol)*2),0,charPerRow*charPerCol*2);
}
void VGACard::displayCharWithProperty()
{
	char charData = pCPU->al;
	U1 charProperty = pCPU->bl;
	unsigned int page = pCPU->bh;
	string str(pCPU->cx, charData);//构造出内容为cx个charData的string
	insertString(str, charProperty, cursor[page].x, cursor[page].y, page, true);

}
void VGACard::displayChar()
{
	char charData = pCPU->al;
	unsigned int page = pCPU->bh;
	string str(pCPU->cx, charData);//构造出内容为cx个charData的string
	insertString(str, MASK_00001111/*黑底白字*/, cursor[page].x, cursor[page].y, page, true);
}
void VGACard::setPalette(){}
void VGACard::displayPixel(){}
void VGACard::displayString()
{
	char *p = (char*)(memStartAddr + pCPU->bp + (pCPU->es <<4));
	this->insertString(string(p, pCPU->cl), pCPU->bl, pCPU->dl, pCPU->dh, pCPU->bh, true);
	
}


void VGACard::getVGAInfo()
{
	pCPU->bh = 0;//VGA彩色
	pCPU->bl = 0x3;//适配器内存大小，256K以上
	pCPU->cl = 0x7;//彩色80*25
	//ch 是设置特性连接器的，现在还不知道怎么设置
}

void VGACard::getVGAStatus()
{
	pCPU->ah = this->charPerRow;
	pCPU->al = this->vgaMode;
	pCPU->bh = this->currentPage;
}

void VGACard::characterGenerate()
{
	switch(pCPU->al)
	{
	case 0x30:
		switch(pCPU->bh)
		{
		case 0:
			pCPU->bp = *(U2*)(memStartAddr + 0x1F*4);
			pCPU->es = *(U2*)(memStartAddr + 0x1F*4+2);
			break;
		case 1:
			pCPU->bp = *(U2*)(memStartAddr + 0x43*4);
			pCPU->es = *(U2*)(memStartAddr + 0x43*4+2);
			break;
		}
		pCPU->cx = 0x10;
		pCPU->dl = this->charPerCol-1;
		break;
	}
}



void VGACard::getCursor_type(){}//no ....
void VGACard::getCursor_position()
{
	if(pCPU->bh < MAX_PAGE)
	{
		pCPU->ch = this->cursor[pCPU->bh].cursor_top;//本来应该是cursor[pCPU->bh],但是奇怪的是bh居然很大~~
		pCPU->cl = this->cursor[pCPU->bh].cursor_bottom;
		pCPU->dh = this->cursor[pCPU->bh].y;
		pCPU->dl = this->cursor[pCPU->bh].x;
	}
}
void VGACard::getCharWithProperty()
{
	unsigned int page = pCPU->bh;
	pCPU->al = *(U1*)(textMemAddr + (page*charPerRow*charPerCol+cursor[page].x + cursor[page].y*charPerRow)*2);//字符
	pCPU->ah = *(U1*)(textMemAddr + (page*charPerRow*charPerCol+cursor[page].x + cursor[page].y*charPerRow)*2 +1);//属性
}
void VGACard::getPixel(){}


void VGACard::insertString(string text, U1 pro,  unsigned int x, unsigned y,
						 unsigned int page , bool isCursorSensitive)
{
	U1 tmpCursor_X = cursor[page].x;
	U1 tmpCursor_Y = cursor[page].y;
	unsigned int pageOffset = (this->charPerCol * this->charPerRow) * 2 * page;
	cursor[page].x = x;
	cursor[page].y = y;
	for(int i=0; i<text.length(); i++)
	{
		switch(text.at(i))
		{
		case 7:
			//assert(false);//响铃，没有处理
			return;
		case 8://光标退格
			cursorBackOneStep(page);
			break;
		case 0xa://需要换行
			if(cursorToNextRow(page))
				screenUpMoveOneLine();
			break;
		case 0xd://无作为
			break;
		default:
			*(textMemAddr + pageOffset + (cursor[page].x  + cursor[page].y * charPerRow)*2) = text.at(i);
			*(textMemAddr + pageOffset + (cursor[page].x  + cursor[page].y * charPerRow)*2 + 1) = pro;
			cursorToNext(page);
		}
		
	}
	if(!isCursorSensitive)
	{
		cursor[page].x = tmpCursor_X;
		cursor[page].y = tmpCursor_Y;
	}

}
void VGACard::screenUpMoveOneLine()
{
	memcpy((void*)(textMemAddr + currentPage*charPerRow*charPerCol*2),
		(void*)(textMemAddr + (currentPage*charPerRow*charPerCol + charPerRow)*2),
		charPerRow*(charPerCol - 1)*2);//显存内容上移
	memset((void*)(textMemAddr + ((currentPage+1)*charPerRow*charPerCol - charPerRow)*2), 0,
		charPerRow*2);//最后一行置零

}
void VGACard::cursorBackOneStep(const unsigned int page)
{
	if(this->cursor[page].x > 0)
	{	//不是第零列，可以回退
		cursor[page].x--;
	}
	else
	{//第零列
		cursor[page].x = charPerRow - 1;
		if(0 == cursor[page].y)
		{//最初试的位置
			cursor[page].x = 0;
			cursor[page].y = 0;
		}
		else
		{
			cursor[page].y--;
		}
	}

}
bool VGACard::cursorToNextRow(const unsigned int page)
{
	cursor[page].x = 0;
	if(cursor[page].y == charPerCol -1)
	{
		return true; //需要向上翻滚
	}
	else
	{
		cursor[page].y++;
		return false;  //没有向上翻滚的趋势
	}
}
void VGACard::cursorToNext(const unsigned int page)
{//光标移动到下一个位置
	if(cursor[page].x == charPerRow -1)
	{
		cursor[page].x = 0;
		if(cursor[page].y == charPerCol -1)
		{
			cursor[page].x = charPerRow -1;
			cursor[page].y = charPerCol -1;
		}
		else
			cursor[page].y++;

	}
	else
	{
		cursor[page].x++;
	}
}
//PropertyString VGACard::getPropertyString(unsigned int x, unsigned int y)
//{
//	PropertyString str;
//	str.pro = getCurrentPro(x,y);
//	char charData=0;
//	while(true)
//	{
//		if(0==(charData = getChar(x, y) )
//			return str;
//		if(++x > charPerRow)
//		{
//			x=0;
//			if(++y > charPerCol)
//				return str;
//		}
//		str.text += charData;
//	}
//}
U1 VGACard::getChar(unsigned int x, unsigned int y)
{
	return *(textMemAddr + (currentPage*charPerRow*charPerCol + y * charPerRow + x)*2);
}
U1 VGACard::getPro(unsigned int x, unsigned int y)
{
	return *(textMemAddr + (currentPage*charPerRow*charPerCol + y * charPerRow + x)*2 +1);
}

