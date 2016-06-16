--TEST--
Bug #24884 (calling $this->__clone(); crashes php)
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class Test
{
    function __copy()
    {
        $string = PHP_VERSION;
        $version = $string[0];
        if ($string < 5) {
            return $this;
        } else {
            return clone $this;
        }
    }
}
function fn1338036559()
{
    $test = new Test();
    $test2 = $test->__copy();
    var_dump($test2);
}
fn1338036559();
--EXPECTF--
object(Test)#%d (0) {
}
