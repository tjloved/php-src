--TEST--
Bug #71414 (Interface method override inherited method and implemented in a trait causes fatal error)
--FILE--
<?php

interface InterfaceY
{
    public function z() : string;
}
trait TraitY
{
    public function z() : string
    {
    }
}
class X
{
    public function z()
    {
    }
}
class Y extends X implements InterfaceY
{
    use TraitY;
}
function fn359729004()
{
    echo "ok";
}
fn359729004();
--EXPECT--
ok
