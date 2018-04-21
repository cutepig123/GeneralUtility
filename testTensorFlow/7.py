import tensorflow as tf
import numpy as np

# 定義一個添加層的函數
def add_layer(inputs, input_tensors, output_tensors, activation_function = None):
    with tf.name_scope('Layer'):
        with tf.name_scope('Weights'):
            W = tf.Variable(tf.random_normal([input_tensors, output_tensors]))
        with tf.name_scope('Biases'):
            b = tf.Variable(tf.zeros([1, output_tensors]))
        with tf.name_scope('Formula'):
            formula = tf.add(tf.matmul(inputs, W), b)
        if activation_function is None:
            outputs = formula
        else:
            outputs = activation_function(formula)
        return outputs

# 準備資料
x_data = np.random.rand(100)
x_data = x_data.reshape(len(x_data), 1)
y_data = x_data * 0.1 + 0.3

# 建立 Feeds
with tf.name_scope('Inputs'):
    x_feeds = tf.placeholder(tf.float32, shape = [None, 1])
    y_feeds = tf.placeholder(tf.float32, shape = [None, 1])

# 添加 1 個隱藏層
hidden_layer = add_layer(x_feeds, input_tensors = 1, output_tensors = 10, activation_function = None)

# 添加 1 個輸出層
output_layer = add_layer(hidden_layer, input_tensors = 10, output_tensors = 1, activation_function = None)

# 定義 `loss` 與要使用的 Optimizer
with tf.name_scope('Loss'):
    loss = tf.reduce_mean(tf.square(y_feeds - output_layer))
with tf.name_scope('Train'):
    optimizer = tf.train.GradientDescentOptimizer(learning_rate = 0.01)
    train = optimizer.minimize(loss)

# 初始化 Graph
init = tf.global_variables_initializer()
sess = tf.Session()

# 將視覺化輸出
writer = tf.summary.FileWriter("TensorBoard/", graph = sess.graph)

# 開始運算
sess.run(init)
for step in range(201):
    sess.run(train, feed_dict = {x_feeds: x_data, y_feeds: y_data})
    #if step % 20 == 0:
        #print(sess.run(loss, feed_dict = {x_feeds: x_data, y_feeds: y_data}))

sess.close()