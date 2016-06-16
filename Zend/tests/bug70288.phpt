--TEST--
Bug #70288 (Apache crash related to ZEND_SEND_REF)
--FILE--
<?php

class A
{
    public function __get($name)
    {
        return new Stdclass();
    }
}
function test(&$obj)
{
    var_dump($obj);
}
function fn854226141()
{
    $a = new A();
    test($a->dummy);
}
fn854226141();
--EXPECTF--
object(stdClass)#%d (0) {
}
