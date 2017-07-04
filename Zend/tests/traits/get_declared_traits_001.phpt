--TEST--
Testing get_declared_traits()
--FILE--
<?php

class a
{
}
interface b
{
}
trait c
{
}
abstract class d
{
}
final class e
{
}
function fn35752658()
{
    var_dump(get_declared_traits());
}
fn35752658();
--EXPECTF--
array(%d) {%A
  [%d]=>
  string(1) "c"
}
