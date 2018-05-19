 -- Meta class
Shape = {area = 0}
-- �����෽�� new
function Shape:new (o,side)
  o = o or {}
  setmetatable(o, self)
  self.__index = self
  side = side or 0
  self.area = side*side;
  return o
end
-- �����෽�� printArea
function Shape:printArea ()
  print("���Ϊ ",self.area)
end

-- ��������
myshape = Shape:new(nil,10)
myshape:printArea()

Square = Shape:new()
-- �����෽�� new
function Square:new (o,side)
  o = o or Shape:new(o,side)
  setmetatable(o, self)
  self.__index = self
  return o
end

-- �����෽�� printArea
function Square:printArea ()
  print("���������Ϊ ",self.area)
end

-- ��������
mysquare = Square:new(nil,10)
mysquare:printArea()

Rectangle = Shape:new()
-- �����෽�� new
function Rectangle:new (o,length,breadth)
  o = o or Shape:new(o)
  setmetatable(o, self)
  self.__index = self
  self.area = length * breadth
  return o
end

-- �����෽�� printArea
function Rectangle:printArea ()
  print("�������Ϊ ",self.area)
end

-- ��������
myrectangle = Rectangle:new(nil,10,20)
myrectangle:printArea()

print('---account class')

   Account = {balance = 0}
    
function Account:new (o)
  o = o or {}
  setmetatable(o, self)
  self.__index = self
  return o
end

function Account:deposit (v)
  self.balance = self.balance + v
end

function Account:withdraw (v)
  if v > self.balance then error"insufficient funds" end
  self.balance = self.balance - v
end

--https://www.lua.org/pil/16.2.html
--[[From that class, we want to derive a subclass SpecialAccount, 
which allows the customer to withdraw more than his balance. 
We start with an empty class that simply inherits all its operations from its base class:
--]]

SpecialAccount = Account:new()

s = SpecialAccount:new{limit=1000.00}
s:deposit(100.00)	

function SpecialAccount:withdraw (v)
  if v - self.balance >= self:getLimit() then
	error"insufficient funds"
  end
  self.balance = self.balance - v
end

function SpecialAccount:getLimit ()
  return self.limit or 0
end

--[[
function s:getLimit ()
  return self.balance * 0.10
end
--]]

s:withdraw(200.00)
