--TEST--
Bug #48693 (Double declaration of __lambda_func when lambda wrongly formatted)
--FILE--
<?php

try {
    $x = create_function('', 'return 1; }');
} catch (ParseException $e) {
    echo "$e\n\n";
}

$y = create_function('', 'function a() { }; return 2;');

try {
    $z = create_function('', '{');
} catch (ParseException $e) {
    echo "$e\n\n";
}

$w = create_function('', 'return 3;');

var_dump(
	$y(),
	$w()
);

?>
--EXPECTF--
exception 'ParseException' with message '%s' in %s(%d) : runtime-created function:1
Stack trace:
#0 %s(%d): create_function('', 'return 1; }')
#1 {main}

exception 'ParseException' with message '%s' in %s(%d) : runtime-created function:1
Stack trace:
#0 %s(%d): create_function('', '{')
#1 {main}

int(2)
int(3)
