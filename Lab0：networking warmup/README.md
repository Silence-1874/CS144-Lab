## 2.Networking by hand
### 2.1 Fetch a Web page
#### 1.
在浏览器中访问(http://cs144.keithw.org/hello);
![](https://gitee.com/silence-zoe/img/raw/master/202203212143335.png)
可以看到就是一个简单的欢迎界面。
#### 2.
这是个手速活。
之前以为是一个一个输入，结果一直没什么效果。
实际上，要快速输入下面这三行，记得最后一行输完后还要按**两次**回车。
```
GET /hello HTTP/1.1
Host: cs144.keithw.org
Connection: close
```
> PS:可以直接复制上面的文字粘贴到shell中

![](https://gitee.com/silence-zoe/img/raw/master/202203212210946.png)
响应体:
````
HTTP/1.1 200 OK
Date: Mon, 21 Mar 2022 13:40:17 GMT
Server: Apache
Last-Modified: Thu, 13 Dec 2018 15:45:29 GMT
ETag: "e-57ce93446cb64"
Accept-Ranges: bytes
Content-Length: 14
Connection: close
Content-Type: text/plain

Hello, CS144!
````