# Changelog

## [1.1.0](https://github.com/frereit/KauMA/compare/v1.0.1...v1.1.0) (2023-12-02)


### Features

* **cz:** Add Polynomial::from_json ([771332e](https://github.com/frereit/KauMA/commit/771332e5ea803aa329bdda792473aa10efd65f50))
* **gcm:** Add arithmetic actions ([2b8e286](https://github.com/frereit/KauMA/commit/2b8e286503eef1788f22e5119a19b0f7fcf80d32))
* **gcm:** Add gcm-poly-add action ([2b8e286](https://github.com/frereit/KauMA/commit/2b8e286503eef1788f22e5119a19b0f7fcf80d32))
* **gcm:** Add gcm-poly-div action ([2b8e286](https://github.com/frereit/KauMA/commit/2b8e286503eef1788f22e5119a19b0f7fcf80d32))
* **gcm:** Add gcm-poly-gcd action ([2b8e286](https://github.com/frereit/KauMA/commit/2b8e286503eef1788f22e5119a19b0f7fcf80d32))
* **gcm:** Add gcm-poly-mod action ([2b8e286](https://github.com/frereit/KauMA/commit/2b8e286503eef1788f22e5119a19b0f7fcf80d32))
* **gcm:** Add gcm-poly-mul action ([2b8e286](https://github.com/frereit/KauMA/commit/2b8e286503eef1788f22e5119a19b0f7fcf80d32))
* **gcm:** Add gcm-poly-pow action ([2b8e286](https://github.com/frereit/KauMA/commit/2b8e286503eef1788f22e5119a19b0f7fcf80d32))
* **gcm:** Add gcm-poly-powmod action ([2b8e286](https://github.com/frereit/KauMA/commit/2b8e286503eef1788f22e5119a19b0f7fcf80d32))

## [1.0.1](https://github.com/frereit/KauMA/compare/v1.0.0...v1.0.1) (2023-12-01)


### Performance Improvements

* **gcm:** Use SSE registers and PCLMULQDQ ([623ea57](https://github.com/frereit/KauMA/commit/623ea57f55bdc35243556ee4c5fefcd1d190604e))

## 1.0.0 (2023-11-29)


### Features

* Add ByteManipulation namespace ([5e77043](https://github.com/frereit/KauMA/commit/5e77043b9bf251cd8fe5adb28ba89b0bf04f6ecf))
* Add ByteManipulation::append_as_bytes ([a6642f2](https://github.com/frereit/KauMA/commit/a6642f2f38652fbe56420c05313e4a772f24de61))
* Add cppcodec library ([07a2a05](https://github.com/frereit/KauMA/commit/07a2a054d91d1abf2a53d2b0dd5ff92d7c12287d))
* **bytenigma:** Add bytenigma glue code ([de8c26c](https://github.com/frereit/KauMA/commit/de8c26c6d4b579b606f7f00abb861f55744ff17d))
* **bytenigma:** Add implementation of public APIs ([789f13f](https://github.com/frereit/KauMA/commit/789f13f10baf1c089fb8d3a243822608a1110dbf))
* **bytenigma:** Add inverted rotors field ([7b1572a](https://github.com/frereit/KauMA/commit/7b1572a5e8bd8b6dc1b99b17648c2250a58c377d))
* **bytenigma:** Implement `backward_pass` ([76a00bd](https://github.com/frereit/KauMA/commit/76a00bd4dd95454ebdef6a487f30281c50a59c27))
* **bytenigma:** Implement `forward_pass` ([17b63ac](https://github.com/frereit/KauMA/commit/17b63acaf8f4b25906b1537fd65cbb7ba62a42a6))
* **bytenigma:** Implement `turn_rotor` ([b6332f8](https://github.com/frereit/KauMA/commit/b6332f8d3375359b5cbf6cdf4d7495f3fd981491))
* **bytenigma:** Move bytenigma to Bytenigma class ([77a95b1](https://github.com/frereit/KauMA/commit/77a95b172483156d2632969453a712320dbba7e9))
* **cz:** Add "cantor-zassenhaus" action ([356966e](https://github.com/frereit/KauMA/commit/356966eb065ccd609eb7fb3ff4fcb4cdd4655d7e))
* **cz:** Add Cantor Zassenhaus implementation ([9b3c286](https://github.com/frereit/KauMA/commit/9b3c286436ca3d168e2cad34760247d022bd955d))
* **cz:** Add CantorZassenhaus::Polynomial::random ([9d578f6](https://github.com/frereit/KauMA/commit/9d578f6e00e6e51506ec44c2dd9ddd40e248f97d))
* **cz:** Add debug logging ([3514d37](https://github.com/frereit/KauMA/commit/3514d377716c87366ca53c46ef5eff87fc0fe009))
* **cz:** Add GCD implementation ([c7a2bf2](https://github.com/frereit/KauMA/commit/c7a2bf25bc86123f2cb4a59a1270f9960bdd03b2))
* **cz:** Add Polynomial::to_json ([8aed8ed](https://github.com/frereit/KauMA/commit/8aed8ed461dfdb1737d2909d865807d2b339ff39))
* **f128:** Add "gcm-clmul" action ([b8adc56](https://github.com/frereit/KauMA/commit/b8adc56540f4cb5b6f0a9ce366d0dbac4b32ece4))
* **f128:** Add addition and multiplication ([5008eec](https://github.com/frereit/KauMA/commit/5008eec8831cf3185b8d29da20abba7e3944423f))
* **f128:** Add conversion actions ([2bbe86b](https://github.com/frereit/KauMA/commit/2bbe86b7de91a677eaf0f2b83bdac5b222928db9))
* **f128:** Add F128_GCM::Polynomial ([0508aed](https://github.com/frereit/KauMA/commit/0508aedb0351e8b57fe5066ab5c74866f694701a))
* **gcm:** Add "gcm-recover" action ([3a2d72f](https://github.com/frereit/KauMA/commit/3a2d72f2245356b0b9bc5f412bc492a6fd30b086))
* **gcm:** Add `&lt;<` operator for Polynomial class ([016b7f9](https://github.com/frereit/KauMA/commit/016b7f9bf4487450ba1d4f562e09a4dfcdedf339))
* **gcm:** Add auxiliary Y0 and H values ([81ff65a](https://github.com/frereit/KauMA/commit/81ff65a2d2a6cab1a6808d47bd607074ff4d1669))
* **gcm:** Add Cantor Zassenhaus polynomial ([7ee4d3a](https://github.com/frereit/KauMA/commit/7ee4d3a3700f51a5e71c91e09a5263d66cd633e4))
* **gcm:** Add encryption ([1bfdc24](https://github.com/frereit/KauMA/commit/1bfdc24213430adc65b251893ba00defdf2570eb))
* **gcm:** Add GCM::Encryptor class ([ba13424](https://github.com/frereit/KauMA/commit/ba13424d50ffd54ca9b532f7ee8496fca5c6df17))
* **gcm:** Add GCM::encryptor::authenticate ([edbd5c4](https://github.com/frereit/KauMA/commit/edbd5c4d58fb5415d5ff8e37ce36205c35986eb6))
* **gcm:** Add GCM::Encryptor::encrypt_and_authenticate ([b8641b7](https://github.com/frereit/KauMA/commit/b8641b789c70e51c1e879ecb98188ce4bd90a1ce))
* **gcm:** Add GCM::Polynomial::random ([7198e4a](https://github.com/frereit/KauMA/commit/7198e4a3c33cfecaceeec833184a0fce37567501))
* **gcm:** Add ghash implementation ([f4bb92e](https://github.com/frereit/KauMA/commit/f4bb92ef8ce74a71e6bcd701113e4c4e3c8ab21b))
* **gcm:** Add glue code for "gcm-encrypt" action ([15cf516](https://github.com/frereit/KauMA/commit/15cf5160560dd3eb49a9a2869400b6c3b9d254f8))
* **gcm:** Add polynomial inverse and division ([2b426fe](https://github.com/frereit/KauMA/commit/2b426fe4ef163ffbe65f0365b99e320fd218e926))
* **gcm:** Add streamable GCM implementation ([243eec9](https://github.com/frereit/KauMA/commit/243eec9fcf33b215458cc33ab4b9749fad25a5ad))
* **gcm:** Add streamable GHASH implementation ([2f4a136](https://github.com/frereit/KauMA/commit/2f4a1367f5d068e4953e456848382031d2ccfbdb))
* **gcm:** Allow for arbitrary sized data ([cf3fe79](https://github.com/frereit/KauMA/commit/cf3fe7998b0cae2f3090a234eef8b21298634f6a))
* **gcm:** Allow for arbitrary sized nonce ([e0b398d](https://github.com/frereit/KauMA/commit/e0b398dc5e3ae00b989a526b033c73e89abc4146))
* **gz:** Add Polynomial::ensure_monic ([b03ef56](https://github.com/frereit/KauMA/commit/b03ef56699cf67f6f78e1fdf1015a9e7b0b8764b))
* **gz:** Add Polynomial::pow ([ff1f60d](https://github.com/frereit/KauMA/commit/ff1f60dd05ed525b8a4ab890b886783ec3fd1d25))
* **noop:** Add 'noop' action ([c654d1f](https://github.com/frereit/KauMA/commit/c654d1fa9a10d6eec1289a083a5176d4836c4279))
* **padding_oracle:** Add attack header ([825ddb8](https://github.com/frereit/KauMA/commit/825ddb823830348aa36d1823d62adba677706384))
* **padding_oracle:** Add attack implementation ([9cb3e2c](https://github.com/frereit/KauMA/commit/9cb3e2cc15eb0bb1b26023d76e99e8c55f31b1f1))
* **padding_oracle:** Add glue code for padding-oracle-attack ([45e745f](https://github.com/frereit/KauMA/commit/45e745ffb4780117808686573b6f59d44405f8b4))
* **padding_oracle:** Add glue code for server ([3d917df](https://github.com/frereit/KauMA/commit/3d917df4878732b4c520966418e92b6f3cb78f8a))
* **padding_oracle:** Add XOR-CBC padding oracle ([37580d0](https://github.com/frereit/KauMA/commit/37580d07a5e31ae43b460834a0598f746e2f099f))
* **tcp:** Add TCP server abstract class ([8dcbcdb](https://github.com/frereit/KauMA/commit/8dcbcdb0dd69bb3cbff9ad9eaeade79b49674dd7))
* **tcp:** TCP::Communicator from hostname & port ([10be5cb](https://github.com/frereit/KauMA/commit/10be5cbf326f766f4d7bcdeb8f60887e68f0a2e3))


### Bug Fixes

* Add .dockerignore ([f350b1e](https://github.com/frereit/KauMA/commit/f350b1e99244df6b8688aae31fd5527007d2d79d))
* Add missing header guards ([9e2bca1](https://github.com/frereit/KauMA/commit/9e2bca1e12219b259e809d39f7456609269416ae))
* **bench:** Make benchmarking script executable ([e542f23](https://github.com/frereit/KauMA/commit/e542f230e079e687e65d1d8be9d625b715401b54))
* **bytenigma:** Fix incorrect usage of size_t ([dd351f0](https://github.com/frereit/KauMA/commit/dd351f029deda0773e4d4e2f749a737259a1510a))
* **bytenigma:** Use std::ranges for for-loops ([8502337](https://github.com/frereit/KauMA/commit/85023375163a8f0a15bba6c265860ae5a611bcc9))
* **cz:** Ensure Polynomials are normalized ([15b960c](https://github.com/frereit/KauMA/commit/15b960cddab96894d66268f69c91e7e168cbd6ad))
* **cz:** Fix degree of random polynomials ([22bbae1](https://github.com/frereit/KauMA/commit/22bbae1d3af80a8b311332042d2b388e177605b5))
* **cz:** Fix divmod implementation errors ([08d705c](https://github.com/frereit/KauMA/commit/08d705ccf179a78f7b8a1ff7ddad63621eb27184))
* **cz:** Fix factor accumulation ([cce4efc](https://github.com/frereit/KauMA/commit/cce4efc7b5d1154272a21001291148ad8b2b39bf))
* **cz:** Fix polynomial equality check ([30e85c1](https://github.com/frereit/KauMA/commit/30e85c10de7196599adda40e378692efdc061b35))
* **cz:** Fix Polynomial::to_json ([950f4ad](https://github.com/frereit/KauMA/commit/950f4ad57c02aa718af7746845fb97d28e876d25))
* **docs:** Fix heading indentation ([c4b6bdb](https://github.com/frereit/KauMA/commit/c4b6bdbe90bc4578ec846c50509c6d46318dc753))
* **f128:** Add #pragma once to header ([9f973c3](https://github.com/frereit/KauMA/commit/9f973c3a7d13f761ac3a37f6fd8f1dad49647322))
* **gcm:** Add missing imports ([076b5b2](https://github.com/frereit/KauMA/commit/076b5b286667201213a0fcfa72202a54f1f71e79))
* **gcm:** Fix order of ghash blocks in GCM::Recovery ([093ad60](https://github.com/frereit/KauMA/commit/093ad601d8a7b5070c6b2e6cc210c06dd7c11295))
* **gcm:** Fix recovery to use a third message as validation ([a7ec74a](https://github.com/frereit/KauMA/commit/a7ec74a62616a52ca1e37b75745a6824029c6052))
* **padding_oracle:** Fix type of PaddingOracle::BLOCK_SIZE ([93d73e1](https://github.com/frereit/KauMA/commit/93d73e1732f4e626f50b9a138999e998d534f2aa))
* **padding_oracle:** Reanme server action ([e8ba5c6](https://github.com/frereit/KauMA/commit/e8ba5c6d069706116df83ddc64af12d60d5ce1d9))
* Remove erroneous testcase in root ([93a58a9](https://github.com/frereit/KauMA/commit/93a58a9cea9d7be7a496be6a621ec1c520debce9))
* **test:** Fix deadlock if stdout exceeds stdout buffer ([fa5f738](https://github.com/frereit/KauMA/commit/fa5f73815c2b70c2208a76a7dfab351bfc00b8af))


### Performance Improvements

* Add benchmarking script ([3e00d99](https://github.com/frereit/KauMA/commit/3e00d99b2f5ddf7e64f84eea28bbbaea42ed22bd))
* **bytenigma:** Remove unneccessary vector copies ([000c9d3](https://github.com/frereit/KauMA/commit/000c9d374fd78f2bce1e25ce55b5776982ad095f))


### Miscellaneous Chores

* release 1.0.0 ([021940b](https://github.com/frereit/KauMA/commit/021940b26abe9ff03af1ecd9bd6fe05caf0c668d))
