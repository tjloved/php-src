--TEST--
Return type for internal functions 2

--SKIPIF--
<?php
if (!function_exists('zend_test_func2')) {
    print 'skip';
}

--FILE--
<?php

function fn779383760()
{
    zend_test_func2();
    echo "==DONE==\n";
}
fn779383760();
--EXPECTF--
==DONE==
