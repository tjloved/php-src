<?php
declare(strict_types=1);
error_reporting(E_ALL);

$opts = parseCliArgs();
if (!isset($opts['file'])) {
    die("No file specified\n");
}

$file = $opts['file'];
if (!file_exists($file)) {
    die("File does not exist\n");
}

$lines = readLines($file);
$instrs = parseTrace($lines);
if (isset($opts['opcode'])) {
    $instrs = filterInstrs($instrs, createOpcodeFilter($opts['opcode']));
}
if (isset($opts['next'])) {
    $instrs = filterInstrsNext($instrs, createOpcodeFilter($opts['next']));
}
if (isset($opts['op1'])) {
    $instrs = filterOperand($instrs, 'op1', 'use_info', $opts['op1']);
}
if (isset($opts['op1-def'])) {
    $instrs = filterOperand($instrs, 'op1', 'def_info', $opts['op1-def']);
}
if (isset($opts['op2'])) {
    $instrs = filterOperand($instrs, 'op2', 'use_info', $opts['op2']);
}
if (isset($opts['op2-def'])) {
    $instrs = filterOperand($instrs, 'op2', 'def_info', $opts['op2-def']);
}
if (isset($opts['result'])) {
    $instrs = filterOperand($instrs, 'result', 'def_info', $opts['result']);
}
if (isset($opts['result-use'])) {
    $instrs = filterOperand($instrs, 'result-use', 'use_info', $opts['result-use']);
}

$showTypes = !isset($opts['no-types']);
printCounts(stringifyInstrs($instrs, $showTypes));

function parseCliArgs() : array {
    $opts = getopt('f:o:1:2:r:n', [
        'file:', 'opcode:', 'op1:', 'op2:', 'result:',
        'op1-def:', 'op2-def:', 'result-use', 'no-types',
        'next:',
    ]);
    $shortToLong = [
        'f' => 'file', 'o' => 'opcode',
        '1' => 'op1', '2' => 'op2', 'r' => 'result',
        'n' => 'no-types',
    ];
    foreach ($shortToLong as $short => $long) {
        if (isset($opts[$short])) {
            $opts[$long] = $opts[$short];
            unset($opts[$short]);
        }
    }
    return $opts;
}

function readLines(string $file) : Traversable {
    $fd = fopen($file, 'r');
    while (false !== $line = fgets($fd)) {
        yield $line;
    }
}

function parseTrace($lines) : Traversable {
    foreach ($lines as $line) {
        $regex = <<<REGEX
/^ZEND_(\w+) ([A-Z]+) (-?\d+) (-?\d+) ([A-Z]+) (-?\d+) (-?\d+) ([A-Z]+) (-?\d+) (-?\d+)$/
REGEX;
        if (!preg_match($regex, $line, $matches)) {
            continue;
        }

        yield new Instr(
            $matches[1],
            new Operand($matches[2], (int) $matches[3], (int) $matches[4]),
            new Operand($matches[5], (int) $matches[6], (int) $matches[7]),
            new Operand($matches[8], (int) $matches[9], (int) $matches[10])
        );
    }
}

function createOpcodeFilter(string $opcodeSpec) : Closure {
    $negated = false;
    if ($opcodeSpec[0] === '!') {
        $negated = true;
        $opcodeSpec = substr($opcodeSpec, 1);
    }

    $opcodes = array_flip(explode('|', $opcodeSpec));
    return function($instr) use($opcodes, $negated) {
        return isset($opcodes[$instr->opcode]) xor $negated;
    };
}

function filterInstrs($instrs, callable $filter) : Traversable {
    foreach ($instrs as $instr) {
        if ($filter($instr)) {
            yield $instr;
        }
    }
}

/* Filter instructions by checking *next* instruction */
function filterInstrsNext($instrs, callable $filter) : Traversable {
    $prev = null;
    foreach ($instrs as $instr) {
        if ($filter($instr) && $prev !== null) {
            yield $prev;
        }
        $prev = $instr;
    }
}

function filterOperand($instrs, string $opName, string $field, string $opSpec) {
    $checkOpType = in_array($opSpec, ['CONST', 'TMP', 'CV', 'VAR', 'UNUSED']);
    if (!$checkOpType) {
        $info = Type::fromString($opSpec);
    }
    foreach ($instrs as $instr) {
        $op = $instr->$opName;
        if ($checkOpType) {
            if ($op->op_type === $opSpec) {
                yield $instr;
            }
        } else {
            if ($op->$field != Type::UNUSED && ($op->$field & $info)) {
                yield $instr;
            }
        }
    }
}

function stringifyInstrs($instrs, bool $showTypes = true) : Traversable {
    foreach ($instrs as $instr) {
        yield $instr->format($showTypes);
    }
}

function printCounts($strings) : void {
    $counts = [];
    foreach ($strings as $string) {
        if (!isset($counts[$string])) {
            $counts[$string] = 1;
        } else {
            $counts[$string]++;
        }
    }

    asort($counts);

    foreach ($counts as $string => $count) {
        echo "$count: $string\n";
    }
    echo "Total: ", array_sum($counts), "\n";
}

class Operand {
    public $op_type;
    public $use_info;
    public $def_info;

    public function __construct(string $op_type, int $use_info, int $def_info) {
        $this->op_type = $op_type;
        $this->use_info = $use_info;
        $this->def_info = $def_info;
    }

    public function format(bool $showTypes) : string {
        if ($this->op_type === 'UNUSED' || !$showTypes) {
            return $this->op_type;
        }

        $result = $this->op_type . '(';
        if ($this->use_info !== Type::UNUSED) {
            $result .= Type::format($this->use_info);
        }
        if ($this->def_info !== Type::UNUSED) {
            if ($this->use_info !== Type::UNUSED) {
                $result .= ' ';
            }
            if ($this->def_info === $this->use_info) {
                $result .= '-> .';
            } else {
                $result .= '-> ' . Type::format($this->def_info);
            }
        }
        return $result . ')';
    }

    public function isUnused() : bool {
        return $this->op_type === 'UNUSED';
    }
}

class Instr {
    public $opcode;
    public $op1;
    public $op2;
    public $result;

    public function __construct(string $opcode, Operand $op1, Operand $op2, Operand $result) {
        $this->opcode = $opcode;
        $this->op1 = $op1;
        $this->op2 = $op2;
        $this->result = $result;
    }

    public function format(bool $showTypes) : string {
        $result = '';
        if (!$this->result->isUnused()) {
            $result .= $this->result->format($showTypes) . ' = ';
        }

        $result .= $this->opcode
            . ' ' . $this->op1->format($showTypes)
            . ' ' . $this->op2->format($showTypes);
        return $result;
    }
}

class Type {
    const UNDEF    = 1 << 0;
    const NULL     = 1 << 1;
    const FALSE    = 1 << 2;
    const TRUE     = 1 << 3;
    const LONG     = 1 << 4;
    const DOUBLE   = 1 << 5;
    const STRING   = 1 << 6;
    const ARRAY    = 1 << 7;
    const OBJECT   = 1 << 8;
    const RESOURCE = 1 << 9;
    const REF      = 1 << 10;

    const BOOL = self::FALSE | self::TRUE;
    const ANY = self::NULL | self::FALSE | self::TRUE
        | self::LONG | self::DOUBLE | self::STRING
        | self::ARRAY | self::OBJECT | self::RESOURCE;

    private const ARRAY_SHIFT = 10;
    const ARRAY_OF_NULL     = self::NULL     << self::ARRAY_SHIFT;
    const ARRAY_OF_FALSE    = self::FALSE    << self::ARRAY_SHIFT;
    const ARRAY_OF_TRUE     = self::TRUE     << self::ARRAY_SHIFT;
    const ARRAY_OF_LONG     = self::LONG     << self::ARRAY_SHIFT;
    const ARRAY_OF_DOUBLE   = self::DOUBLE   << self::ARRAY_SHIFT;
    const ARRAY_OF_STRING   = self::STRING   << self::ARRAY_SHIFT;
    const ARRAY_OF_ARRAY    = self::ARRAY    << self::ARRAY_SHIFT;
    const ARRAY_OF_OBJECT   = self::OBJECT   << self::ARRAY_SHIFT;
    const ARRAY_OF_RESOURCE = self::RESOURCE << self::ARRAY_SHIFT;
    const ARRAY_OF_ANY      = self::ANY      << self::ARRAY_SHIFT;

    const ARRAY_KEY_LONG   = 1 << 21;
    const ARRAY_KEY_STRING = 1 << 22;
    const ARRAY_KEY_ANY    = self::ARRAY_KEY_LONG | self::ARRAY_KEY_STRING;

    const ERROR = 1 << 23;
    const CLAZZ = 1 << 24;

    const UNUSED = -1;

    private const TO_NAME = [
        self::UNDEF    => "undef",
        self::NULL     => "null",
        self::FALSE    => "false",
        self::TRUE     => "true",
        self::LONG     => "long",
        self::DOUBLE   => "double",
        self::STRING   => "string",
        self::ARRAY    => "array",
        self::OBJECT   => "object",
        self::RESOURCE => "resource",
        self::BOOL     => "bool",
        self::ANY      => "any",
        self::REF      => "ref",
        self::ERROR    => "error",
        self::CLAZZ    => "class",
    ];

    public static function fromString(string $spec) : int {
        $nameToBit = array_flip(self::TO_NAME);
        $parts = explode('|', $spec);
        $result = 0;
        foreach ($parts as $part) {
            if (isset($nameToBit[$part])) {
                $result |= $nameToBit[$part];
            } else if (preg_match('/array\[(?:([\w|]+)=>)?([\w|]+)\]/', $part, $matches)) {
                $result |= self::ARRAY;
                $result |= self::fromString($matches[2]) << self::ARRAY_SHIFT;
                if ($matches[1] === 'long') {
                    $result |= self::ARRAY_KEY_LONG;
                } else if ($matches[1] === 'string') {
                    $result |= self::ARRAY_KEY_STRING;
                } else {
                    assert($matches[1] === '');
                    $result |= self::ARRAY_KEY_ANY;
                }
            } else {
                throw new Exception("Unknown type part $part");
            }
        }
        return $result;
    }

    private static function setBits(int $num) : Traversable {
        $i = 0;
        for ($i = 0; $i < PHP_INT_SIZE * 8 - 1; $i++) {
            if ($num & (1 << $i)) {
                yield 1 << $i;
            }
        }
    }

    private static function formatSingleType(int $type) : string {
        if (!isset(self::TO_NAME[$type])) {
            throw new Exception("Unknown type $type");
        }

        return self::TO_NAME[$type];
    }

    private static function formatTypeUnion(int $info) : string {
        if ($info & self::REF) {
            return 'ref';
        }

        if (($info & self::ANY) === self::ANY) {
            return 'any';
        }

        $types = [];
        foreach (self::setBits($info & self::ANY) as $type) {
            $types[] = self::formatSingleType($type);
        }

        $string = implode('|', $types);
        return str_replace('false|true', 'bool', $string);
    }

    public static function format(int $info) : string {
        $types = [];
        if ($info & self::UNDEF) {
            $types[] = 'undef';
        }
        if ($info & self::ANY) {
            $types[] = self::formatTypeUnion($info);
        }
        if ($info & self::ERROR) {
            $types[] = 'error';
        }
        if ($info & self::CLAZZ) {
            $types[] = 'class';
        }

        $string = implode('|', $types);

        if ($info & self::ARRAY_OF_ANY) {
            $arrayString = 'array[';
            if (($info & self::ARRAY_KEY_ANY) != self::ARRAY_KEY_ANY) {
                if ($info & self::ARRAY_KEY_LONG) {
                    $arrayString .= 'long=>';
                } else {
                    $arrayString .= 'string=>';
                }
            }
            $arrayString .= self::formatTypeUnion($info >> self::ARRAY_SHIFT) . ']';
            $string = str_replace('array', $arrayString, $string);
        }

        return $string;
    }
}
