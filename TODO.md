# Resolve symbols

逻辑是遍历所有的objfiles，然后把每一个global且不是undef的添加到全局变量表上

# MarkLiveObjs

遍历 alive 的 object files，如果中间某个undef，出现定义，就将其加入到 alive 集合中