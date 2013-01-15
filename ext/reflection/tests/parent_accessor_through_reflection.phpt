--TEST--
The parent accessor can be accessed through Reflection
--XFAIL--
Doesn't actually go through parent accessors :/
--FILE--
<?php

class Test {
    public $foo {
        get { echo __METHOD__."()\n"; return $this->foo; }
        set { echo __METHOD__."($value)\n"; $this->foo = $value; }
    }
}

class Test2 extends Test {
    public $foo {
        get {
            echo __METHOD__."()\n";
            return (new ReflectionProperty(get_parent_class(), 'foo'))->getValue($this);
        }
        set {
            echo __METHOD__."($value)\n";
            (new ReflectionProperty(get_parent_class(), 'foo'))->setValue($this, $value);
        }
    }
}

$test = new Test2;
$test->foo = 'value';
var_dump($test->foo);

?>
--EXPECT--
