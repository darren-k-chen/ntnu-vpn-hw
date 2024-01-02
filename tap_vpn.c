#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

int createTapDevice(char *dev) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("open /dev/net/tun failed");
        exit(1);
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("TAP created: failed");
        close(fd);
        exit(1);
    }

    strcpy(dev, ifr.ifr_name);
    return fd;
}

void encryptData(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext) {
    AES_KEY enc_key;
    AES_set_encrypt_key(key, 128, &enc_key);
    AES_encrypt(plaintext, ciphertext, &enc_key);
}

void decryptData(unsigned char *ciphertext, unsigned char *key, unsigned char *plaintext) {
    AES_KEY dec_key;
    AES_set_decrypt_key(key, 128, &dec_key);
    AES_decrypt(ciphertext, plaintext, &dec_key);
}

int main() {
    char tap_name[IFNAMSIZ] = "tap0";
    int tap_fd = createTapDevice(tap_name);
    printf("TAP: %s created\n", tap_name);

    unsigned char key[16];
    RAND_bytes(key, sizeof(key));

    unsigned char buffer[1500];
    unsigned char encrypted[1500];
    unsigned char decrypted[1500];

    while (1) {
        int nread = read(tap_fd, buffer, sizeof(buffer));
        if (nread < 0) {
            perror("load data from tap failed");
            close(tap_fd);
            exit(1);
        }

        encryptData(buffer, nread, key, encrypted);
        printf("encrypt %d bytes data \n", nread);

        decryptData(encrypted, key, decrypted);
        printf("decrypt %d bytes data \n", nread);
    }

    return 0;
}

