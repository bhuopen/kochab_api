import os
import time
import http.client
import http.cookies
import xml.etree.ElementTree

class CgiOpr:
    'For BHU Kochab system cgi operation class'

    def __init__(self, addr, user='admin', pwd='admin'):
        self.conn = http.client.HTTPConnection(addr)
        self.user = user;
        self.pwd = pwd;
        self.cookie = http.cookies.SimpleCookie()
        self.cookie["sid"] = ""; #Kochab Bug: If no sid, cmd return "failed" instead of "need login".
        self.dbg = False

    def __del__(self):
        self.conn.close()

    def debug(self, d):
        self.dbg = d

    def __postform(self, vals=None, form=None, file=None, filefname='default', filename='default', filetype='application/octet-stream'):
        boundary = '----%s' % (str(hex(int(time.time() * 1000)))*2)
        data = []
        data.append(bytes('--{}'.format(boundary), encoding = "utf8"))
        if form : data.extend([bytes('Content-Disposition: form-data; name="{}"\r\n{}--{}'.format(k, v, boundary), encoding = "utf8") for k, v in form.items()])
        if file:
            with open(file,'rb') as f:
                data.append(bytes('Content-Disposition: form-data; name="{}"; filename="{}"'.format(filefname, filename, boundary), encoding = "utf8"))
                data.append(bytes('Content-Type: {}\r\n'.format(filetype), encoding = "utf8"))
                data.append(f.read())
                data.append(bytes('--{}'.format(boundary), encoding = "utf8"))
        http_body = b'\r\n'.join(data)
        http_body += b'--\r\n'
        if vals:
            parm="?"+"&".join(['{}={}'.format(k, v) for k,v in vals.items()])
        else:
            parm=""
        self.conn.request("POST", "/cgi-bin/cgiSrv.cgi"+parm, http_body,
                {'Content-Type':'multipart/form-data; boundary={}'.format(boundary),
                 'Accept':'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
                 'Cookie':self.cookie.output(header="")})
        r1 = self.conn.getresponse()
        if r1.status == 200: status = True
        else: status = False
        return [status, r1.getheaders(), r1.read()]

    def __op(self, op, args=None):
        if isinstance(args, dict):
            for key,value in args.items():
                op += '&'+key+'='+value
        self.conn.request("POST", "/cgi-bin/cgiSrv.cgi", "op="+op,
                {'Content-Type':'application/x-www-form-urlencoded',
                 'Cookie':self.cookie.output(header="")})
        r1 = self.conn.getresponse()
        if r1.status == 200: status = True
        else: status = False
        return [status, r1.getheaders(), r1.read()]

    def __login(self):
        ret = self.__op('user')
        if self.dbg : print("Cgi Check User: "+ret)
        if ret[0] == True and str(ret[2]).find('result=failed') != -1:
            ret = self.__op('login', {'user':self.user, 'password':self.pwd})
            if self.dbg : print("Cgi Login:"+self.user+"="+self.pwd+"\nret:"+ret)
            if ret[0] == False or str(ret[2]).find('result=ok') == -1:
                return False
            for i in range(len(ret[1])):
                if ret[1][i][0] == 'Set-Cookie': 
                    self.cookie.load(ret[1][i][1])
                    break
            return True
        return False

    def __get(self, path):
        self.conn.request("GET", "/cgi-bin/cgiSrv.cgi?xml=["+path+"]", None,
                {'Content-Type':'text/xml',
                 'Cookie':self.cookie.output(header="")})
        r1 = self.conn.getresponse()
        if r1.status == 200: status = True
        else: status = False
        if self.dbg : print("Cgi Get:"+path+"\nret:"+r1.status+"\nheader:"+r1.getheaders())
        return [status, r1.getheaders(), r1.read()]

    def __xml(self, xml):
        self.conn.request("POST", "/cgi-bin/cgiSrv.cgi", xml,
                {'Content-Type':'text/xml',
                 'Cookie':self.cookie.output(header="")})
        r1 = self.conn.getresponse()
        if r1.status == 200: status = True
        else: status = False
        if self.dbg : print("Cgi xml:"+xml+"\nret:"+r1.status+"\nheader:"+r1.getheaders())
        return [status, r1.getheaders(), r1.read()]

    def __cmd(self, cmd):
        self.conn.request("GET", "/cgi-bin/cgiSrv.cgi?xml=["+cmd+"]", None,
                {'Content-Type':'text/xml',
                 'Cookie':self.cookie.output(header="")})
        r1 = self.conn.getresponse()
        if r1.status == 200: status = True
        else: status = False
        if self.dbg : print("Cgi Get:"+path+"\nret:"+r1.status+"\nheader:"+r1.getheaders())
        return [status, r1.getheaders(), r1.read()]

    def logout(self):
        ret = self.__op('logout')
        if ret[0] == False or str(ret[2]).find('result=ok') == -1:
            return False
        self.cookie = http.cookies.SimpleCookie();
        self.cookie["sid"] = "";
        return True

    def login(self, user=None, pwd=None):
        if user is not None:
            self.user = user
        if pwd is not None:
            self.pwd = pwd
        if len(self.cookie["sid"]) > 0: # Logou first
            logout();
        return self.__login();

    def __xmlop(self, func):
        ret = func()
        if ret[0] == False:
            return [False, 'Connection Error']
        root = xml.etree.ElementTree.fromstring(ret[2])
        if root.tag == 'return':
            for item in root.findall('ITEM'):
                result = item.attrib.get('result')
                if result != "need_login":
                    break
                elif not self.__login():
                    return [False, 'Login Failed']
                break
        else:
            return [True, ret[2]]
        ret = func()
        if ret[0] == False:
            return [False, 'Connection Error2']
        root = xml.etree.ElementTree.fromstring(ret[2])
        if root.tag == 'return':
            for item in root.findall('ITEM'):
                result = item.attrib.get('result')
                if result == "need_login" or result == "failed":
                    return [False, "Return:"+item]
        return [True, ret[2]]

    def get(self, path):
        def _get(self, path):
            def _func():
                return self.__get(path)
            return _func
        ret = self.__xmlop(_get(self, path))
        return ret

    def apply(self, xml):
        def _apply(self, xml):
            def _func():
                return self.__xml(xml)
            return _func
        ret = self.__xmlop(_apply(self, xml))
        return ret

    def cmd(self, xml):
        return self.apply(xml)

    def op(self, op, args=None):
        ret = self.__op(op, args)
        if ret[0] == False:
            return [False, 'Connection Error']
        if str(ret[2]).find('need_login') != -1:
            if not self.__login():
                return [False, 'Login Failed']
        else:
            return [True, ret[2]]
        ret = self.__op(op, args)
        if ret[0] == False or str(ret[2]).find('result=ok') == -1:
            return [False, 'Apply Failed']
        return [True, ret[2]]
	
    def upgruade(self, file):
        self.__login()
        if os.path.exists(file):
            return self.__postform(vals={'upload':'[image]'}, file=file, filefname="fwfile", filename=file.rsplit("/")[-1])
        else:
            return [False, "No such file: "+file]
        

if __name__ == "__main__":
    print("BHU Kochab CGI Operation Class. Testing...")
    cgi = CgiOpr("192.168.62.1")
    print("\n Save op:")
    print(cgi.op("save"))
    print("\n Get dev.net.interface:")
    print(cgi.get("dev.net.interface"))
    print("\n wlanstatus cmd:")
    print(cgi.cmd('<cmd>\n<ITEM cmd="wlanstatus" interface="wlan0"/>\n</cmd>'))
    print("\n upgurade:")
    print(cgi.upgruade(r'image'))
    print("\n Logout:")
    print(cgi.logout())
	
if __name__ == "t":
	None
