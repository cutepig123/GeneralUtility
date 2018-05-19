 -- Meta class
Shape = {area = 0}
-- 基础类方法 new
function Shape:new (o,side)
  o = o or {}
  setmetatable(o, self)
  self.__index = self
  side = side or 0
  self.area = side*side;
  return o
end
-- 基础类方法 printArea
function Shape:printArea ()
  print("面积为 ",self.area)
end

-- 创建对象
myshape = Shape:new(nil,10)
myshape:printArea()

Square = Shape:new()
-- 派生类方法 new
function Square:new (o,side)
  o = o or Shape:new(o,side)
  setmetatable(o, self)
  self.__index = self
  return o
end

-- 派生类方法 printArea
function Square:printArea ()
  print("正方形面积为 ",self.area)
end

-- 创建对象
mysquare = Square:new(nil,10)
mysquare:printArea()

Rectangle = Shape:new()
-- 派生类方法 new
function Rectangle:new (o,length,breadth)
  o = o or Shape:new(o)
  setmetatable(o, self)
  self.__index = self
  self.area = length * breadth
  return o
end

-- 派生类方法 printArea
function Rectangle:printArea ()
  print("矩形面积为 ",self.area)
end

-- 创建对象
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

print('--new Account with privacy')
--https://www.lua.org/pil/16.4.html
function newAccount (initialBalance)
  local self = {balance = initialBalance}

  local withdraw = function (v)
					 self.balance = self.balance - v
				   end

  local deposit = function (v)
					self.balance = self.balance + v
				  end

  local getBalance = function () return self.balance end

  return {
	withdraw = withdraw,
	deposit = deposit,
	getBalance = getBalance
  }
end

    acc1 = newAccount(100.00)
    acc1.withdraw(40.00)
    print(acc1.getBalance())     --> 60
	

print('--The Single-Method Approach')
function newObject (value)
  return function (action, v)
	if action == "get" then return value
	elseif action == "set" then value = v
	else error("invalid action")
	end
  end
end
--Its use is straightforward:
d = newObject(0)
print(d("get"))    --> 0
d("set", 10)
print(d("get"))    --> 10

