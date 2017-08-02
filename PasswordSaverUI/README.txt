Encryptor.cpp member function details

generateHaystack()
- "Generate the haystack" by creating 3 key[num].bin
   files.  Each file is created by inserting 10016
   randomly generated characters and 8 random numerical
   digits at the end, resulting each file size of 10024 characters each

generateKeyFile()
- A 16 character binary file(key.bin) is created that
  would be used to encrypt or decrypt the "haystack"
  and obtain information as to whether how the encryption is done
  The first character is assigned 'T' if a PIN was used and 'F' if
  a password was used for encryption.  The second character is assigned
  a random number between 1 and 3, which specifies which key[num].bin file
  to truly configure for encryption and use for decryption

configureHaystack()
- Configures the haystack by changing the specified key[num].bin file's
  digit to be configured for encryption.
  The 8 digits are configured by:
   -Assigning the 1st, 3rd, 5th, and 7th digits
    random digits to be used as a four-digit index of the IV in the key[num].bin
   -Assigning the 2nd, 4th, 6th, and 8th digits
    digits obtained from XORing the digits of the PIN and a randomly generated number(0-1000) if PIN is used
    or from XORing the digits of the IV index and the random number if a password is used.

generateIV()
- Generates the main file IV by combining the 1st, 3rd, 5th, and 7th digits
  to specify the starting character index.  Then 16 subsequent characters
  starting from that index are read to generate a 16-char IV

generateKey()
- Generates the main file key by:
    - XORing the PIN digits(PIN method) or IV index(password method) with the
      arbitrary digits(2nd, 4th, 6th, and 8th) and taking the modulus of 10 to generate the Key index
      Example:
	  - PIN: 1234
	  - arbDigits: 5678
	  - (1^5) % 10, (2^6) % 10, (3^7) % 10, (4^8) % 10
    - The arbitrary digits are then used to specify which 4 blocks of 4 characters to
      use to make the 16-char key
      Exmaple:
          - Key: 1324, arbDigits: 1, 4, 5, 2
          - key = block of 4[index: 1324 + 1]
                   + block of 4[index: 1324 + 4]
                   + block of 4[index: 1324 + 5]
                   + block of 4[index: 1324 + 2]