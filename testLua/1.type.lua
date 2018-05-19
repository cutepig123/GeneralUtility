print(type("Hello world"))      --> string
print(type(10.4*3))             --> number
print(type(print))              --> function
print(type(type))               --> function
print(type(true))               --> boolean
print(type(nil))                --> nil
print(type(type(X)))            --> string

print('--nil')

tab1 = { key1 = "val1", key2 = "val2", "val3" }
for k, v in pairs(tab1) do
    print(k .. " - " .. v)
end
 
print('--nil2')
tab1.key1 = nil
for k, v in pairs(tab1) do
    print(k .. " - " .. v)
end

print('--nil 作比较时应该加上双引号 "：')
print(type(X)==nil)
print(type(X)=='nil')

print('--boolean 类型只有两个可选值：true（真） 和 false（假），Lua 把 false 和 nil 看作是"假"，其他的都为"真":')
print(type(true))
print(type(false))
print(type(nil))
 
if false or nil then
    print("至少有一个是 true")
else
    print("false 和 nil 都为 false!")
end

html = [[
<html>
<head></head>
<body>
    <a href="http://www.runoob.com/">菜鸟教程</a>
</body>
</html>
]]
print(html)

print('--+')
print("2" + 6)
print("2" + "6")
print("-2e2" * "6")


print('--字符串连接使用的是 ..')
print("a" .. 'b')

print('--使用 # 来计算字符串的长度')
len = "www.runoob.com"
print(#len)

print('-- 创建一个空的 table')
local tbl1 = {}
 
-- 直接初始表
local tbl2 = {"apple", "pear", "orange", "grape"}

a = {}
a["key"] = "value"
key = 10
a[key] = 22
a[key] = a[key] + 11

for k, v in pairs(a) do
    print(k .. " : " .. v)
end

local b = {"apple", "pear", "orange", "grape"}
for k, v in pairs(b) do
    print(k .. " : " .. v)
end

print('--中文测试')
a="我是中国人"
 
function get_chinese_char(str, index)
local start = (index-1) * 3 + 1
return str:sub(start, start + 2)
end
 
print(get_chinese_char(a, 3));
print(string.len(a))
print(#a)

print(setmetatable({},{__tostring=function() return "字符串" end}))

print('--function（函数）')
-- function_test.lua 脚本文件
function factorial1(n)
    if n == 0 then
        return 1
    else
        return n * factorial1(n - 1)
    end
end
print(factorial1(5))
factorial2 = factorial1
print(factorial2(5))

print('--function 可以以匿名函数（anonymous function）的方式通过参数传递:')

-- function_test2.lua 脚本文件
function testFun(tab,fun)
    for k ,v in pairs(tab) do
        print(fun(k,v));
    end
end

--为啥, x=yxxx不能打印出来呢？
tab={key1="val1",key2="val2", x=yxxx};
testFun(tab,
function(key,val)--匿名函数
    return key.."="..val;
end
);

print('--strict')
require 'strict'
--print(invalid_var)

-- test.lua 文件脚本
a = 5               -- 全局变量
local b = 5         -- 局部变量

c=nil
d=nil
function joke()
    c = 5           -- 全局变量
    local d = 6     -- 局部变量
end

joke()
print(c,d)          --> 5 nil

do 
    local a = 6     -- 局部变量
    b = 6           -- 对局部变量重新赋值
    print(a,b);     --> 6 6
end

print(a,b)      --> 5 6

a = "hello" .. "world"
t={n=1}
t.n = t.n + 1
t.x=1

i=1
while( i<10 )
do
   print("循环将永远执行下去")
   i = i+1
end

--[ 0 为 true ]
if(0)
then
    print("0 为 true")
end


print('--以下实例定义了函数 max()，参数为 num1, num2，用于比较两值的大小，并返回最大值：')

--[[ 函数返回两个值的最大值 --]]
function max(num1, num2)
	local result = nil
   if (num1 > num2) then
      result = num1;
   else
      result = num2;
   end

   return result; 
end
-- 调用函数
print("两值比较最大值为 ",max(10,4))
print("两值比较最大值为 ",max(5,6))

print('--Lua 函数可以接受可变数目的参数，和 C 语言类似，在函数参数列表中使用三点 ... 表示函数有可变的参数。')

function add(...)  
local s = 0  
  for i, v in ipairs{...} do   --> {...} 表示一个由所有变长参数构成的数组  
    s = s + v  
  end  
  return s  
end  
print(add(3,4,5,6,7))  --->25

function average(...)
   local result = 0
   local arg={...}    --> arg 为一个表，局部变量
   for i,v in ipairs(arg) do
      result = result + v
   end
   print("总共传入 " .. #arg .. " 个数")
   return result/#arg
end

print("平均值为",average(10,5,3,4,5,6))

function average(...)
   local result = 0
   local arg={...}
   for i,v in ipairs(arg) do
      result = result + v
   end
   print("总共传入 " .. select("#",...) .. " 个数")
   return result/select("#",...)
end

print("平均值为",average(10,5,3,4,5,6))

a = 21
b = 10
c = a + b
print("Line 1 - c 的值为 ", c )
c = a - b
print("Line 2 - c 的值为 ", c )
c = a * b
print("Line 3 - c 的值为 ", c )
c = a / b
print("Line 4 - c 的值为 ", c )
c = a % b
print("Line 5 - c 的值为 ", c )
c = a^2
print("Line 6 - c 的值为 ", c )
c = -a
print("Line 7 - c 的值为 ", c )

a = 21
b = 10

if( a == b )
then
   print("Line 1 - a 等于 b" )
else
   print("Line 1 - a 不等于 b" )
end

if( a ~= b )
then
   print("Line 2 - a 不等于 b" )
else
   print("Line 2 - a 等于 b" )
end

if ( a < b )
then
   print("Line 3 - a 小于 b" )
else
   print("Line 3 - a 大于等于 b" )
end

if ( a > b ) 
then
   print("Line 4 - a 大于 b" )
else
   print("Line 5 - a 小于等于 b" )
end

-- 修改 a 和 b 的值
a = 5
b = 20
if ( a <= b ) 
then
   print("Line 5 - a 小于等于  b" )
end

if ( b >= a ) 
then
   print("Line 6 - b 大于等于 a" )
end

a = true
b = true

if ( a and b )
then
   print("a and b - 条件为 true" )
end

if ( a or b )
then
   print("a or b - 条件为 true" )
end

print("---------分割线---------" )

-- 修改 a 和 b 的值
a = false
b = true

if ( a and b )
then
   print("a and b - 条件为 true" )
else
   print("a and b - 条件为 false" )
end

if ( not( a and b) )
then
   print("not( a and b) - 条件为 true" )
else
   print("not( a and b) - 条件为 false" )
end

print('--Lua 字符串')
print(string.gsub("aaaa","a","z",3))	--> zzza    3

