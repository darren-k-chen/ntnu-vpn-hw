# 612K0025C 陳冠儒 資安攻防 vpn 程式碼
## 使用指南
### 系統
```
Ubuntu 22.04 LTS
```
### 安裝方式
#### 安裝依賴庫
```bash
sudo apt update
sudo apt -y install openssl
sudo apt -y install libssl-dev
```
#### 編譯程式碼
```bash
gcc -o tap_vpn tap_vpn.c -lssl -lcrypto
```
#### 執行程式
```bash
chmod 775 tap_vpn; sudo ./tap_vpn
```
