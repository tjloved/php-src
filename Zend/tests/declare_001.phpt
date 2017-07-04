--TEST--
Testing declare statement with several type values
--SKIPIF--
<?php
if (!extension_loaded("mbstring")) {
  die("skip Requires ext/mbstring");
}
?>
--INI--
zend.multibyte=1
--FILE--
<?php

declare (encoding=1);
declare (encoding=1.1231312321313124E+20);
declare (encoding='utf-8');
declare (encoding=M_PI);
function fn12407645()
{
    print 'DONE';
}
fn12407645();
--EXPECTF--
Warning: Unsupported encoding [1] in %sdeclare_001.php on line %d

Warning: Unsupported encoding [1.1231312321313E+20] in %sdeclare_001.php on line %d

Fatal error: Encoding must be a literal in %s on line %d
