--TEST--
Testing declare statement with several type values
--INI--
zend.multibyte=1
--FILE--
<?php

declare (encoding=1);
declare (encoding=1123131232131312321);
declare (encoding=M_PI);
function fn986414234()
{
    print 'DONE';
}
fn986414234();
--EXPECTF--
Warning: Unsupported encoding [%d] in %sdeclare_004.php on line %d

Warning: Unsupported encoding [%f] in %sdeclare_004.php on line %d

Fatal error: Encoding must be a literal in %sdeclare_004.php on line %d
