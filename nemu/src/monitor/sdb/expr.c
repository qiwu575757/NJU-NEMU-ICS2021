#include <isa.h>
#include <memory/paddr.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
  TK_NOTYPE = 0x41, TK_EQ, 
  NUM, HEX, TK_UEQ, REG, DEREF, MINUS
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {//这里面不要有字符的type，因为标识从A开始
  {"0x[0-9A-Fa-f]+", HEX}, //16进制数字
  {"\\$[0-9a-z]+", REG},//寄存器
  {"[0-9]+",NUM},       // 数字
  {"\\(", '('},         // 左括号
  {"\\)", ')'},         // 右括号
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // sub
  {"\\*", '*'},         // mul
  {"\\/", '/'},         // divide
  {" +", TK_NOTYPE},    // spaces
  {"==", TK_EQ},        // equal
  {"!=", TK_UEQ},
  {"&&", '&'},
  {"\\|\\|", '|'}
};

#define NR_REGEX ARRLEN(rules)



static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;
  
  assert(ARRLEN(rules) < 26);

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[2048] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

int priority(char type);
word_t eval(int p, int q, bool *success, int *position);

#define STACK_SIZE 1024 
#define DEFAULT_POSITION -1
#define priorityROTY_BASE 16


static bool make_token(const char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        const char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        
        if (substr_len > 32){
						panic("Substr len is > 32."); 
        }

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;
        
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed. 
         */

        switch (rules[i].token_type) {
          case '*':
          case '-':
            if (nr_token == 0 || tokens[nr_token - 1].type == '(' || priority(tokens[nr_token - 1].type) > 0){
              switch (rules[i].token_type)
              {
              case '*':
                tokens[nr_token].type = DEREF;
                break;
              case '-':
                tokens[nr_token].type = MINUS;
                break;
              }
            }else if (tokens[nr_token - 1].type == ')' 
              || tokens[nr_token - 1].type == NUM || tokens[nr_token - 1].type == HEX
              || tokens[nr_token - 1].type == REG){
              tokens[nr_token].type = rules[i].token_type;
            }else {
								panic("Get error str.");
						}
            nr_token++;
            break;

          case TK_NOTYPE:
            break;
          case NUM:
          case HEX:
          case REG:
            memcpy(tokens[nr_token].str, e + position - substr_len, (substr_len) * sizeof(char));
            tokens[nr_token].str[substr_len] = '\0';
            // IFDEF(CONFIG_DEBUG, Log("[DEBUG ]读入了一个数字%s", tokens[nr_token].str));
          default: 
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

word_t expr(const char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  *success = true;
  int position = 0;
  int ans = eval(0, nr_token - 1, success, &position);
  if (!*success){
    printf("some problem happens at position %d\n%s\n%*.s^\n", position, e, position, "");
  }
  return ans;
}

bool check_parentheses(int p, int q, int *position){
  char stack[STACK_SIZE];
  *position = DEFAULT_POSITION;
  int top = -1, index = p;
  bool is_parentheses = tokens[p].type == '(';
  while (index <= q){
    if (tokens[index].type == '('){
      stack[++top] = '(';
    }else if (tokens[index].type == ')'){
      if (top < 0 || stack[top] != '('){
        *position = p;
        return false;
      }else {
        top--;
      }
    }
    if (index < q)
      is_parentheses = (top >= 0) && is_parentheses; // 永远都该有一个前括号
    index++;
  }
  if (top != -1){ //栈空
    *position = p;
    return false;
  }
  return is_parentheses;
}

int priority(char type){
  switch (type) {
    case '|':
      return priorityROTY_BASE + 0;

    case '&':
      return priorityROTY_BASE + 1;

    case TK_EQ:
    case TK_UEQ:
      return priorityROTY_BASE + 2;

    case '+':
    case '-':
      return priorityROTY_BASE + 3;
    
    case '*':
    case '/':
      return priorityROTY_BASE + 4;

    default:
      return -1;
  }
}

u_int32_t eval(int p, int q, bool *success, int *position) {
  if (p > q) {
    *success = false;
    return 0;
  } else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    u_int32_t buffer = 0;
    switch (tokens[p].type){
      case HEX:
        sscanf(tokens[p].str, "%x", &buffer);
        break;
      
      case NUM:
        sscanf(tokens[p].str, "%u", &buffer);
        break;

      case REG:
        if (strcmp(tokens[p].str, "$pc") == 0){
          buffer = cpu.pc;
          *success = true;
        }else {
          buffer = isa_reg_str2val(tokens[p].str, success);
        }

        if (!*success){
          *position = p;
          return 0;
        }
        break;

      default:
          panic("Single token get error token.");
		}
    // IFDEF(CONFIG_DEBUG, Log("读取数据 %d %s %x", buffer, tokens[p].str, tokens[p].type));
    return buffer;
  }else if (q - p == 1 || check_parentheses(p + 1, q, position) == true){//长度为2的子表达式呈型于 -[NUM] *[NUM]
    switch (tokens[p].type) {
      case DEREF:
        return *((uint32_t *)guest_to_host(eval(p + 1, q, success, position)));
        break;
      
      case MINUS://取负
        return -eval(p + 1, q, success, position);
      default:
          panic("长度为2的子表达式解析错误。");
		}
  } else if (check_parentheses(p, q, position) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    // IFDEF(CONFIG_DEBUG, Log("解括号"));
    return eval(p + 1, q - 1, success, position);
  } else {
    // IFDEF(CONFIG_DEBUG, Log("计算"));
    if (*position != DEFAULT_POSITION){
      *success = false;
      return 0;
    }
    int op = -1, level = -1;
    for (int i = p; i <= q; ++i){
      if (tokens[i].type == '('){
        level++;
      } else if (tokens[i].type == ')'){
        level--;// 不再检查合法性，一定合法
      } else if (level == -1 && priority(tokens[i].type) >= 0){//说明层次不在括号里且是运算符
        if (op == -1 || priority(tokens[i].type) <= priority(tokens[op].type)){// 寻找主运算符
          op = i;
        }
      }
    }
    if (op == -1){
      *success = false;
      *position = 0;
      return 0;
    }

    // IFDEF(CONFIG_DEBUG, Log("主运算符 %c", tokens[op].type));
    u_int32_t val1 = eval(p, op - 1, success, position);
    u_int32_t val2 = eval(op + 1, q, success, position);
    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_UEQ: return val1 != val2;
      case '|': return val1 || val2;
      case '&': return val1 && val2;
      default: panic("主运算符出错"); 
    }
  }
  return 0;
}
