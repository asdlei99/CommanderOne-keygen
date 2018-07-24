# CommanderOne keygen

## 1. How to build

Before you build patcher and keygen, you should make sure you have installed OpenSSL.

If you have brew, you can install it by `brew install openssl`.

  * Build patcher

    ```bash
    $ cd code
    $ make patcher
    ```

  * Build keygen

    ```bash
    $ cd code
    $ make keygen
    ```

## 2. How to use

  1. Build patcher and keygen.

  2. Run patcher to relpace the official RSA pubic key with our own public key. In console:

     ```bash
     $ ./patcher <your CommanderOne MachO file> [RSA-2048 Private Key (PEM file)]
     ```

     Example:

     ```bash
     $ ./patcher /Applications/Commander\ One.app/Contents/MacOS/Commander\ One
     Your RSA public key is

     -----BEGIN PUBLIC KEY-----
     MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEArAYs/AGjtyqBSluE4xLI
     aoB4dMaeA+G0lTjkdrOtSC0hdytQ7jGNdhNcfOEOZ3sNL7ebzEVeY1yW7OdBBehN
     FJw+BbY3H/PVcAm/d4s3Wv8XlsiAJrjQkPusouEs6xbIIKUs4ckwRXiBgJZFgZpV
     MWCJc86PalfIy4aH/YTydBMdy/xs0SiNu6eouSfH6PmRwkadHW6eTm2iPc6E/Xt2
     +EBblwXFDSFzOk5+q7RQS10DkI2jJ8HagIGV8Tej9UINYrEYZs1jo+J79akCX6lB
     tzCPUFdX835sdoDoNhcglztnYKxM4/4waaXcRQlpnHLRCLXknMDT30GdT5vPpTvT
     ewIDAQAB
     -----END PUBLIC KEY-----

     Target file: /Applications/Commander One.app/Contents/MacOS/Commander One
     Open file successfully!
     File size: 4909376 byte(s).
     Map file successfully!
     offset = 0x0000000000399390, writing data.....Patch is done.
     Modified: 1
     ```

     You will get `private.pem` at current directory.

  3. Re-codesign Commander One. Because we made a patch to `Commander One.app`, its original code signatures became invalid. So we have to re-codesign it. In console:

     ```bash
     $ codesign -f -s "Your code-sign certificate name" <path to Commander One.app>
     ```

     __NOTICE:__ "your code-sign certificate name" should be the name of your code-sign certificate, which is displayed in `Keychain.app`.

  4. Run keygen to generate an activation code:

     ```bash
     $ ./keygen
     ```

     Example:

     ```bash
     $ ./keygen
     Activation Code:
     92630-8CBF4-8F613-AD6A9-59A04
     ```

  5. Disconnect network and open `Commander One.app`. Find and click `Activate PRO Pack`.

     ![](pic0.png)

     Then input activation code that keygen gave and click `Activate`.

  6. Generally, online activation would failed. But it doesn't matter. Click `Activate manually`.

     ![](pic1.png)

  7. You will see

     ![](pic2.png)

     Click `Save activation info` first and save activation info.

  8. Run keygen again. But this time, please specify the paths to the file you just save and `private.pem`. In console:

     ```bash
     $ ./keygen <path to Commander One.activate> <path to private.pem>
     ```

     You will be asked to input your name. Just input it. After that you will `License.bin` at current directory.

     Example:

     ```bash
     $ ./keygen ~/Commander\ One.activate private.pem
     Your RSA private key is

     -----BEGIN RSA PRIVATE KEY-----
     MIIEogIBAAKCAQEArAYs/AGjtyqBSluE4xLIaoB4dMaeA+G0lTjkdrOtSC0hdytQ
     7jGNdhNcfOEOZ3sNL7ebzEVeY1yW7OdBBehNFJw+BbY3H/PVcAm/d4s3Wv8XlsiA
     JrjQkPusouEs6xbIIKUs4ckwRXiBgJZFgZpVMWCJc86PalfIy4aH/YTydBMdy/xs
     0SiNu6eouSfH6PmRwkadHW6eTm2iPc6E/Xt2+EBblwXFDSFzOk5+q7RQS10DkI2j
     J8HagIGV8Tej9UINYrEYZs1jo+J79akCX6lBtzCPUFdX835sdoDoNhcglztnYKxM
     4/4waaXcRQlpnHLRCLXknMDT30GdT5vPpTvTewIDAQABAoIBACEYuxTtN9No0SE+
     RG3v0jFzcDl/cS1sAnNnU8VmwmMNk8+6vMJnNEFQbk30evH74ciZedcuTX+uc7c2
     svLxA8uAWAp/SVHTdvAsX/MfDNJsG9y9zLImgrIySb+4tQyyAGUPgngaXjgvfDMC
     NnM9/0AVw7xNNJiQNSi7RHTJXGBqcDQtGfj1RBymHeG6Fy/e8nLNKVahfD2A6mmL
     ix0Xon1ZBaEItw82GGnTi1zhy6d8dQQ4Wryhp1L8Ylb4nhWp1YiJqGqlsbU5KuXU
     CjuVB/kFFPQqiqnAfNgTc6UNn+tfkW8+KyszLF0q5pFkXmIqmQaUEK9fcbq0uapL
     bks1xQECgYEA4KBxjHRmqtHFyWwiD2UeytoNEaux5il2iaDvQkruzczC9aFkD18u
     t6CS8TbVMORndVG2q3QrBw8Y1et3HuzY+exJVM5efCrMSgoTrGE3V55c3rdsDHlp
     dpdXOl0CiGGJzVrHGU3T9+30DQuHLNdF4lohdLQtbauj0CamhiKFOfsCgYEAxAzr
     X5RXjvE6Mx1aAAXaCiKk8cu0AxACaBbqAfqhTA+VS5rpPDAl3QAfDbOf8wOihtI2
     G6275mnNLqVlIomZjjTnc0VPh2asqGNaDM83Bkv4DY8+rrtp6o0xmljNBvZhFYJr
     4grcAI/Zm8/FsDjnG0iyn06Yc9zKrwet/dQWFIECgYApO4pcW+mUpZkG1kCSTt+O
     IOSRP9ESDL5jmRUSOCsJv/JxoER7xd8Pefw/Ju+PX/3VvUAozNQxIYZPjj95f8R8
     a/EyDgeuBzUkyiC+XcII5VrqzwSYbZj8Wd97KSOT4UOoXGd7r5ufQSCxXFJd2s5L
     NHw7re9A2GbjiJS8vo1yDQKBgDDWnxPyWlWLlloBZ9g1nPmFY92wRym0JUVU3/qa
     OvA9uY1aC9woimYhj5rUMnZRY31+hGgkwXhGxwxlu2n9KTmZyEaY75TlaQGRrm8e
     d7Cj4ClQk+bnXR1HCyJOGNUxL0IyeTmYRvIE48rC2ZZqtne/hm/VHnlOeVdc7GUd
     HlsBAoGAOmNb5ILCTDlLtcqAP4gxKu4rIAMVh53AkXJKKbaw4NBFl6d+qQvrC5kU
     RCx3WfAvEB3Qq7XgKVy57zhAZWmCdYlDDbewaOjyUn9O3QOKai+03HNTUdgbxJrA
     fr+qyjFW4vdCLddS+6It3uLF+KUPNWwjHCNiQYZf6nzlqg7fKUc=
     -----END RSA PRIVATE KEY-----

     Input your name:Double Sine
     License.bin is generated successfully.
     ```

  9. Finally, click `Load activation file` and select `License.bin` that you got. If nothing wrong, activation should be done successfully.
