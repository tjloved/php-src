--TEST--
Bug #39297 (Memory corryption because of indirect modification of overloaded array)
--FILE--
<?php

function compareByRef(&$first, &$second)
{
    return $first === $second;
}
class MyTree implements ArrayAccess
{
    public $parent;
    public $children = array();
    public function offsetExists($offset)
    {
    }
    public function offsetUnset($offset)
    {
    }
    public function offsetSet($offset, $value)
    {
        echo "offsetSet()\n";
        $cannonicalName = strtolower($offset);
        $this->children[$cannonicalName] = $value;
        $value->parent = $this;
    }
    public function offsetGet($offset)
    {
        echo "offsetGet()\n";
        $cannonicalName = strtolower($offset);
        return $this->children[$cannonicalName];
    }
}
function fn383914641()
{
    $id = 'Test';
    $root = new MyTree();
    $child = new MyTree();
    $root[$id] = $child;
    var_dump(compareByRef($root[$id], $child));
}
fn383914641();
--EXPECT--
offsetSet()
offsetGet()
bool(true)
