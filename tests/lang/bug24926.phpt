--TEST--
Bug #24926 (lambda function (create_function()) cannot be stored in a class property)
--FILE--
<?php

class foo
{
    public $functions = array();
    function __construct()
    {
        $function = create_function('', 'return "FOO\\n";');
        print $function();
        $this->functions['test'] = $function;
        print $this->functions['test']();
        // werkt al niet meer
    }
}
function fn616414925()
{
    error_reporting(E_ALL);
    $a = new foo();
}
fn616414925();
--EXPECTF--
Deprecated: Function create_function() is deprecated in %s on line %d
FOO
FOO
