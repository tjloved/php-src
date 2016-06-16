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
function fn1523853484()
{
    var_dump(new Foo());
}
fn1523853484();
--EXPECTF--
object(Foo)#%d (%d) {
  [""]=>
  int(1)
}

