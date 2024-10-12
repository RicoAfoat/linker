# 描述

文件定义

假设有

文件格式 A

```
{
    [name]      :[大小(字节)]
    type        :2
    machine     :2
    version     :4
    entry       :4
    phoff       :8
    ...
}
```

文件格式 B
```
{
    [name]      :[大小(字节)]
    something   :4
    anything    :1
    nothing     :8
    ...
}
```

假设有很多种像这样的**文件格式**

有没有什么**开发工具**可以解析这类文件格式

# 实际问题是


