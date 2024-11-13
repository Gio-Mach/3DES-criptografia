#include <stdio.h>
#include <string.h>

#define TAMANHO_DES 8

// Função de substituição simples
void simple_des(const unsigned char *key, const unsigned char *data, unsigned char *saida) {
    // XOR com a chave (simplificado para o exemplo)
    int i;
    for(i = 0; i < TAMANHO_DES; i++) {
        saida[i] = data[i] ^ key[i % TAMANHO_DES]; // XOR básico
    }
}

// Função de permutação simples (apenas para simular algum comportamento de permutação)
void permutate(unsigned char *data) {
    unsigned char temp[TAMANHO_DES];
    int i;
    for(i = 0; i < TAMANHO_DES; i++) {
        temp[i] = data[TAMANHO_DES - 1 - i]; // Inversão do bloco
    }
    memcpy(data, temp, TAMANHO_DES);
}

// Função simplificada para criptografia DES (substituição e permutação)
void des_encrypt(const unsigned char *key, const unsigned char *data, unsigned char *cript) {
    simple_des(key, data, cript); // XOR com a chave
    permutate(cript);             // Simulação de permutação
}

// Função simplificada para descriptografia DES (substituição e permutação inversa)
void des_decrypt(const unsigned char *key, const unsigned char *data, unsigned char *decript) {
    unsigned char temp[TAMANHO_DES];
    memcpy(temp, data, TAMANHO_DES);
    permutate(temp);                  // Desfaz a permutação
    simple_des(key, temp, decript);  // XOR com a chave
}

// Função para a criptografia usando 3DES
void triple_des_encrypt(const unsigned char *key, const unsigned char *data, unsigned char *cript) {
    unsigned char temp1[TAMANHO_DES], temp2[TAMANHO_DES];
    unsigned char key1[TAMANHO_DES], key2[TAMANHO_DES], key3[TAMANHO_DES];

    // Copiando as três chaves (8 bytes cada)
    memcpy(key1, key, TAMANHO_DES);
    memcpy(key2, key + TAMANHO_DES, TAMANHO_DES);
    memcpy(key3, key + 2 * TAMANHO_DES, TAMANHO_DES);

    des_encrypt(key1, data, temp1);
    des_decrypt(key2, temp1, temp2);
    des_encrypt(key3, temp2, cript);
}

// Função para a descriptografia usando 3DES
void triple_des_decrypt(const unsigned char *key, const unsigned char *cript, unsigned char *decript) {
    unsigned char temp1[TAMANHO_DES], temp2[TAMANHO_DES];
    unsigned char key1[TAMANHO_DES], key2[TAMANHO_DES], key3[TAMANHO_DES];

    // Copiando as três chaves
    memcpy(key1, key, TAMANHO_DES);
    memcpy(key2, key + TAMANHO_DES, TAMANHO_DES);
    memcpy(key3, key + 2 * TAMANHO_DES, TAMANHO_DES);

    des_decrypt(key3, cript, temp1);
    des_encrypt(key2, temp1, temp2);
    des_decrypt(key1, temp2, decript);
}

void apply_padding(unsigned char *data, size_t len) {
    // Aplica o padding se o tamanho do bloco for inferior a 8 no final
    size_t pad_len = TAMANHO_DES - len;
    size_t i;
    for (i = len; i < TAMANHO_DES; i++) {
        data[i] = (unsigned char) pad_len;
    }
}

void remove_padding(unsigned char *data, size_t *len) {
    unsigned char pad_len = data[*len - 1];
    if (pad_len > 0 && pad_len <= TAMANHO_DES) {
        *len -= pad_len;
    }
}

int main() {
    unsigned char key[TAMANHO_DES * 3] = "12345678abcdefgh"; // Chave de 24 bytes
    unsigned char data[TAMANHO_DES]; // Dados a serem criptografados
    unsigned char cript[TAMANHO_DES];
    unsigned char decript[TAMANHO_DES];

    FILE *arq_original;
    FILE *cryp;
    FILE *decryp;

    arq_original = fopen("original.txt", "r");
    if (arq_original == NULL) {
        printf("Não foi possível abrir o arquivo do texto original\n");
        return 1;
    }

    cryp = fopen("cript.txt", "w");
    if (cryp == NULL) {
        printf("Não foi possível abrir o arquivo de criptografia.\n");
        fclose(arq_original);
        return 1;
    }

    decryp = fopen("decript.txt", "w");
    if (decryp == NULL) {
        printf("Não foi possível abrir o arquivo de descriptografia.\n");
        fclose(arq_original);
        fclose(cryp);
        return 1;
    }

    size_t bytes_read, len = 0;
    while ((bytes_read = fread(data, 1, TAMANHO_DES, arq_original)) > 0) {
        if (bytes_read < TAMANHO_DES) {
            apply_padding(data, bytes_read);
        }

        // Criptografar o bloco
        triple_des_encrypt(key, data, cript);

        // Escrever o bloco criptografado no arquivo de criptografia
        int i;
        for(i = 0; i < TAMANHO_DES; i++) {
            fprintf(cryp, "%02X ", cript[i]);
        }
        fprintf(cryp, "\n");

        // Descriptografar o bloco
        triple_des_decrypt(key, cript, decript);

        len = TAMANHO_DES;
        if (bytes_read < TAMANHO_DES) {
            remove_padding(decript, &len);
        }

        // Escrever o bloco descriptografado no arquivo de descriptografia
        fwrite(decript, 1, len, decryp);
    }

    printf("Os arquivos estao prontos\n");

    fclose(arq_original);
    fclose(cryp);
    fclose(decryp);

    return 0;
}

