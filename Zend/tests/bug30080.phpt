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
function fn910960516()
{
    new foo(array(new stdClass()));
}
fn910960516();
--EXPECTF--
array(1) {
  [0]=>
  object(stdClass)#%d (0) {
  }
}
