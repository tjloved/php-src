--TEST--
Bug #70782: null ptr deref and segfault (zend_get_class_fetch_type)
--FILE--
<?php

function fn1725761860()
{
    (-0)::$prop;
}
fn1725761860();
--EXPECTF--
Fatal error: Illegal class name in %s on line %d
