--TEST--
Bug #55754 (Only variables should be passed by reference for ZEND_SEND_PREFER_REF params)
--FILE--
<?php

function fn494130659()
{
    current($arr = array(0 => "a"));
    current(array(0 => "a"));
    current($arr);
    echo "DONE";
}
fn494130659();
--EXPECT--
DONE
