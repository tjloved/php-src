--TEST--
Bug #30080 (Passing array or non array of objects)
--FILE--
<?php

class foo
{
    function __construct($arrayobj)
    {
        var_dump($arrayobj);
    }
}
function fn1838498280()
{
    new foo(array(new stdClass()));
}
fn1838498280();
--EXPECTF--
array(1) {
  [0]=>
  object(stdClass)#%d (0) {
  }
}
