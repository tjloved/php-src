--TEST--
Bug #69537 (__debugInfo with empty string for key gives error)
--FILE--
<?php

class Foo
{
    public function __debugInfo()
    {
        return ['' => 1];
    }
}
function fn1096584352()
{
    var_dump(new Foo());
}
fn1096584352();
--EXPECTF--
object(Foo)#%d (%d) {
  [""]=>
  int(1)
}

