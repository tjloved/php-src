--TEST--
Bug #55578 (Segfault on implode/concat)
--FILE--
<?php

class Foo
{
    public function __toString()
    {
        return 'Foo';
    }
}
function test($options, $queryPart)
{
    return '' . (0 ? 1 : $queryPart);
}
function fn521242542()
{
    $options = array();
    var_dump(test($options, new Foo()));
}
fn521242542();
--EXPECT--
string(3) "Foo"
