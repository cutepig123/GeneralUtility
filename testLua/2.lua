
x=[[
��ƪ������Ҫ������Lua�д���ȫ�ֱ�����С���ɣ���ֹδԤ�ڵ�ȫ�ֱ�����,ʹ�ñ��ķ�������ʵ�ֱ��ⴴ��ȫ�ֱ���,�ڱ���ʹ��ȫ�ֱ����������Ҳ������ʵ�ּ���,��Ҫ�����ѿ��Բο���
Lua ��һ�����Ծ���Ĭ�϶���ı�������ȫ�ֵġ�Ϊ�˱�����һ�㣬������Ҫ�ڶ������ʱʹ�� local �ؼ��֡�

�����������������������ʱ����ֵ�һЩ bug �Ǻ��Ѳ��ҵġ��������ǿ��Բ�ȡһ��С���ɣ��ı䴴��ȫ�ֱ����ķ�ʽ��
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

print([[��������Ĵ��������Ҫ�ٶ���ȫ�ֱ����ͻ�ĵõ�һ��������Ϣ��

����ʱ��ȫ�ֱ����Ǳ���ģ�����һЩȫ�ֺ��������ǿ���ʹ���µĶ��巽ʽ��
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
