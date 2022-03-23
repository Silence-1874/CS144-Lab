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
### 2.2 Send yourself an email
由于没有SUNet ID，强行使用时会出现`550 5.7.1 Relaying denied`。
![](https://gitee.com/silence-zoe/img/raw/master/202203231605607.png)
于是换用QQ邮箱(`smtp.qq.com`)，中间遇到了好多问题，网上找了好久资料才解决，将近花了一个小时，在这里不一一列举了。
1. 连接qq邮箱服务器: `telnet smtp.qq.com smtp`。
  ![](https://gitee.com/silence-zoe/img/raw/master/202203231655442.png)
2. 用`HELO`命令打招呼: `HELO smtp.qq.com smtp`。
  ![](https://gitee.com/silence-zoe/img/raw/master/202203231656519.png)
3. 登录并验证: `AUTH LOGINI`。
  ![](https://gitee.com/silence-zoe/img/raw/master/20220323180809.png)
  `VXNlcm5hbWU6`和`UGFzc3dvcmQ6`经过base64解码后分别为`Username:`和`Password:`。
  此处的`Password`并非密码，而是邮箱的**授权码**。
  分别输入用户名和授权码(**都需要用base64加密后输入，不要输入任何多余的空格**)。
4. 输入发送方和接收方(**`<` 和 `>` 不能省！**）。
  ![](https://gitee.com/silence-zoe/img/raw/master/202203231658677.png)
5. 输入`DATA`，准备发送数据。
  ![](https://gitee.com/silence-zoe/img/raw/master/202203231711436.png)
6. 输入`From`, `To`和`Subject: `，空一行后输入正文，最后以一个`.`表示邮件结束，发送邮件。
  ![](https://gitee.com/silence-zoe/img/raw/master/202203231728846.png)
7. 最后输入`QUIT`退出，结束会话。
  ![](https://gitee.com/silence-zoe/img/raw/master/202203231714322.png)
8. 检查邮箱，发现邮件成功发送。
  ![](https://gitee.com/silence-zoe/img/raw/master/202203231734710.png)
