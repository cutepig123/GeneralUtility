# Introduction

测试tensorflow。

# 代码目的，来源，简单介绍,关键词

1.py 测试session https://hackmd.io/s/HJxsUvOpg

2.py 說明如何建立多個 Graph https://hackmd.io/s/HJxsUvOpg

3.py 提供一個簡單的累加器 https://hackmd.io/s/HJxsUvOpg
	tf.Variable tf.assign
	
4.py 一次取得多個節點的 Tensor
	
	
5.py 测试tf.placeholder。

6.py 改寫後的程式 https://ithelp.ithome.com.tw/articles/10187814

7.py 視覺化  https://ithelp.ithome.com.tw/articles/10187814
接著我們要在模組化的程式中使用 with tf.name_scope(): 為每個運算元命名，然後在神經網絡運算初始之後，利用 tf.summary.FileWriter() 將視覺化檔案輸出。

注：原文命令tensorboard --logdir='TensorBoard/'在我这里运行会出错，解决防范是去掉引号，即g:\Python36\Scripts\tensorboard --logdir=TensorBoard

premade_estimator.py	鸢尾花分类问题 不在这里，具体参考	https://www.tensorflow.org/get_started/get_started_for_beginners
