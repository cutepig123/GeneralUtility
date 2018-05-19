
x=[[
这篇文章主要介绍了Lua中创建全局变量的小技巧（禁止未预期的全局变量）,使用本文方法可以实现避免创建全局变量,在必须使用全局变量的情况下也给出了实现技巧,需要的朋友可以参考下
Lua 有一个特性就是默认定义的变量都是全局的。为了避免这一点，我们需要在定义变量时使用 local 关键字。

但难免会出现遗忘的情况，这时候出现的一些 bug 是很难查找的。所以我们可以采取一点小技巧，改变创建全局变量的方式。
]]

print(x)


local __g = _G
-- export global variable
cc={}
cc.exports = {}
setmetatable(cc.exports, {
    __newindex = function(_, name, value)
        rawset(__g, name, value)
    end,

    __index = function(_, name)
        return rawget(__g, name)
    end
})

-- disable create unexpected global variable
setmetatable(__g, {
    __newindex = function(_, name, value)
        local msg = "USE 'cc.exports.%s = value' INSTEAD OF SET GLOBAL VARIABLE"
        error(string.format(msg, name), 0)
    end
})

print([[增加上面的代码后，我们要再定义全局变量就会的得到一个错误信息。

但有时候全局变量是必须的，例如一些全局函数。我们可以使用新的定义方式：
]])

-- export global
cc.exports.MY_GLOBAL = "hello"
-- use global
print(MY_GLOBAL)
-- or
print(_G.MY_GLOBAL)
-- or
print(cc.exports.MY_GLOBAL)

-- delete global
cc.exports.MY_GLOBAL = nil

-- global function
local function test_function_()
end
cc.exports.test_function = test_function_

-- if you set global variable, get an error
--INVALID_GLOBAL = "no"
