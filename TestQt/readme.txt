Test 2
http://devbean.blog.51cto.com/448512/194031
http://devbean.blog.51cto.com/448512/241186

http://devbean.blog.51cto.com/448512/243546
connection��
drawLineAction -->drawLineActionTriggered ->changeCurrentShape -->setCurrentShape
drawRectAction -->drawRectActionTriggered ->changeCurrentShape
events��
mousePressEvent -�� new Rect -��shapeList<<shape shape->setStart(
PaintWidget::mouseMoveEvent --�� shape->setEnd�� update����
mouseReleaseEvent 

//���⣺ò��mainwindow.h/cpp������ɾ����һ��ɾ���ͱ��벻��

flow��
����������
int main
	MainWindow w;
		new PaintWidget(this);
		setCentralWidget(paintWidget);
		
���û��������
drawLineAction
	drawLineActionTriggered
		 emit changeCurrentShape
			setCurrentShape
�û�������
	mousePressEvent
		new Rect;
		shapeList<<shape;
�û��ƶ����
	mouseMoveEvent
		xx
�û��ͷ����	
	mouseReleaseEvent
ˢ�º���
	paintEvent
		shape->paint(painter);
			
			
Test 3
http://devbean.blog.51cto.com/448512/244181
flow��
����������
int main
	MainWindow w;
		new PaintWidget(this);	inherit from QGraphicsScene
		new QGraphicsView(paintWidget, this); 
		setCentralWidget(view);
		
���û��������
drawLineAction
	drawLineActionTriggered
		 emit changeCurrentShape
			setCurrentShape
�û�������
	mousePressEvent
		new Line;
		addItem(line);
		currItem->startDraw(event);
			LIne::startDraw inherit from QGraphicsLineItem
				setLine(QLineF(event->scenePos(), event->scenePos()));
		QGraphicsScene::mousePressEvent(event);
�û��ƶ����
	mouseMoveEvent
		currItem->drawing(event);
		QGraphicsScene::mouseMoveEvent(event);
			 QLineF newLine(line().p1(), event->scenePos());
			setLine(newLine);
�û��ͷ����	
	mouseReleaseEvent
ˢ�º���
	paintEvent
		shape->paint(painter);
		