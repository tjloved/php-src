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
function fn2001037370()
{
    var_dump(get_declared_traits());
}
fn2001037370();
--EXPECT--
array(1) {
  [0]=>
  string(1) "c"
}
