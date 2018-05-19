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

print('--nil ���Ƚ�ʱӦ�ü���˫���� "��')
print(type(X)==nil)
print(type(X)=='nil')

print('--boolean ����ֻ��������ѡֵ��true���棩 �� false���٣���Lua �� false �� nil ������"��"�������Ķ�Ϊ"��":')
print(type(true))
print(type(false))
print(type(nil))
 
if false or nil then
    print("������һ���� true")
else
    print("false �� nil ��Ϊ false!")
end

html = [[
<html>
<head></head>
<body>
    <a href="http://www.runoob.com/">����̳�</a>
</body>
</html>
]]
print(html)

print('--+')
print("2" + 6)
print("2" + "6")
print("-2e2" * "6")


print('--�ַ�������ʹ�õ��� ..')
print("a" .. 'b')

print('--ʹ�� # �������ַ����ĳ���')
len = "www.runoob.com"
print(#len)

print('-- ����һ���յ� table')
local tbl1 = {}
 
-- ֱ�ӳ�ʼ��
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

print('--���Ĳ���')
a="�����й���"
 
function get_chinese_char(str, index)
local start = (index-1) * 3 + 1
return str:sub(start, start + 2)
end
 
print(get_chinese_char(a, 3));
print(string.len(a))
print(#a)

print(setmetatable({},{__tostring=function() return "�ַ���" end}))

print('--function��������')
-- function_test.lua �ű��ļ�
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

print('--function ����������������anonymous function���ķ�ʽͨ����������:')

-- function_test2.lua �ű��ļ�
function testFun(tab,fun)
    for k ,v in pairs(tab) do
        print(fun(k,v));
    end
end

--Ϊɶ, x=yxxx���ܴ�ӡ�����أ�
tab={key1="val1",key2="val2", x=yxxx};
testFun(tab,
function(key,val)--��������
    return key.."="..val;
end
);

print('--strict')
require 'strict'
--print(invalid_var)

-- test.lua �ļ��ű�
a = 5               -- ȫ�ֱ���
local b = 5         -- �ֲ�����

c=nil
d=nil
function joke()
    c = 5           -- ȫ�ֱ���
    local d = 6     -- �ֲ�����
end

joke()
print(c,d)          --> 5 nil

do 
    local a = 6     -- �ֲ�����
    b = 6           -- �Ծֲ��������¸�ֵ
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
   print("ѭ������Զִ����ȥ")
   i = i+1
end

--[ 0 Ϊ true ]
if(0)
then
    print("0 Ϊ true")
end


print('--����ʵ�������˺��� max()������Ϊ num1, num2�����ڱȽ���ֵ�Ĵ�С�����������ֵ��')

--[[ ������������ֵ�����ֵ --]]
function max(num1, num2)
	local result = nil
   if (num1 > num2) then
      result = num1;
   else
      result = num2;
   end

   return result; 
end
-- ���ú���
print("��ֵ�Ƚ����ֵΪ ",max(10,4))
print("��ֵ�Ƚ����ֵΪ ",max(5,6))

print('--Lua �������Խ��ܿɱ���Ŀ�Ĳ������� C �������ƣ��ں��������б���ʹ������ ... ��ʾ�����пɱ�Ĳ�����')

function add(...)  
local s = 0  
  for i, v in ipairs{...} do   --> {...} ��ʾһ�������б䳤�������ɵ�����  
    s = s + v  
  end  
  return s  
end  
print(add(3,4,5,6,7))  --->25

function average(...)
   local result = 0
   local arg={...}    --> arg Ϊһ�����ֲ�����
   for i,v in ipairs(arg) do
      result = result + v
   end
   print("�ܹ����� " .. #arg .. " ����")
   return result/#arg
end

print("ƽ��ֵΪ",average(10,5,3,4,5,6))

function average(...)
   local result = 0
   local arg={...}
   for i,v in ipairs(arg) do
      result = result + v
   end
   print("�ܹ����� " .. select("#",...) .. " ����")
   return result/select("#",...)
end

print("ƽ��ֵΪ",average(10,5,3,4,5,6))

a = 21
b = 10
c = a + b
print("Line 1 - c ��ֵΪ ", c )
c = a - b
print("Line 2 - c ��ֵΪ ", c )
c = a * b
print("Line 3 - c ��ֵΪ ", c )
c = a / b
print("Line 4 - c ��ֵΪ ", c )
c = a % b
print("Line 5 - c ��ֵΪ ", c )
c = a^2
print("Line 6 - c ��ֵΪ ", c )
c = -a
print("Line 7 - c ��ֵΪ ", c )

a = 21
b = 10

if( a == b )
then
   print("Line 1 - a ���� b" )
else
   print("Line 1 - a ������ b" )
end

if( a ~= b )
then
   print("Line 2 - a ������ b" )
else
   print("Line 2 - a ���� b" )
end

if ( a < b )
then
   print("Line 3 - a С�� b" )
else
   print("Line 3 - a ���ڵ��� b" )
end

if ( a > b ) 
then
   print("Line 4 - a ���� b" )
else
   print("Line 5 - a С�ڵ��� b" )
end

-- �޸� a �� b ��ֵ
a = 5
b = 20
if ( a <= b ) 
then
   print("Line 5 - a С�ڵ���  b" )
end

if ( b >= a ) 
then
   print("Line 6 - b ���ڵ��� a" )
end

a = true
b = true

if ( a and b )
then
   print("a and b - ����Ϊ true" )
end

if ( a or b )
then
   print("a or b - ����Ϊ true" )
end

print("---------�ָ���---------" )

-- �޸� a �� b ��ֵ
a = false
b = true

if ( a and b )
then
   print("a and b - ����Ϊ true" )
else
   print("a and b - ����Ϊ false" )
end

if ( not( a and b) )
then
   print("not( a and b) - ����Ϊ true" )
else
   print("not( a and b) - ����Ϊ false" )
end

print('--Lua �ַ���')
print(string.gsub("aaaa","a","z",3))	--> zzza    3

