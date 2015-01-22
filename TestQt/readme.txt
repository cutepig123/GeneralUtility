Test 2
http://devbean.blog.51cto.com/448512/194031
http://devbean.blog.51cto.com/448512/241186

http://devbean.blog.51cto.com/448512/243546
connection：
drawLineAction -->drawLineActionTriggered ->changeCurrentShape -->setCurrentShape
drawRectAction -->drawRectActionTriggered ->changeCurrentShape
events：
mousePressEvent -》 new Rect -》shapeList<<shape shape->setStart(
PaintWidget::mouseMoveEvent --》 shape->setEnd， update（）
mouseReleaseEvent 

//主意：貌似mainwindow.h/cpp不可以删掉，一旦删掉就编译不到

flow：
当程序启动
int main
	MainWindow w;
		new PaintWidget(this);
		setCentralWidget(paintWidget);
		
当用户点击画线
drawLineAction
	drawLineActionTriggered
		 emit changeCurrentShape
			setCurrentShape
用户点击鼠标
	mousePressEvent
		new Rect;
		shapeList<<shape;
用户移动鼠标
	mouseMoveEvent
		xx
用户释放鼠标	
	mouseReleaseEvent
刷新函数
	paintEvent
		shape->paint(painter);
			
			
Test 3
http://devbean.blog.51cto.com/448512/244181
flow：
当程序启动
int main
	MainWindow w;
		new PaintWidget(this);	inherit from QGraphicsScene
		new QGraphicsView(paintWidget, this); 
		setCentralWidget(view);
		
当用户点击画线
drawLineAction
	drawLineActionTriggered
		 emit changeCurrentShape
			setCurrentShape
用户点击鼠标
	mousePressEvent
		new Line;
		addItem(line);
		currItem->startDraw(event);
			LIne::startDraw inherit from QGraphicsLineItem
				setLine(QLineF(event->scenePos(), event->scenePos()));
		QGraphicsScene::mousePressEvent(event);
用户移动鼠标
	mouseMoveEvent
		currItem->drawing(event);
		QGraphicsScene::mouseMoveEvent(event);
			 QLineF newLine(line().p1(), event->scenePos());
			setLine(newLine);
用户释放鼠标	
	mouseReleaseEvent
刷新函数
	paintEvent
		shape->paint(painter);
		