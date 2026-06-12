import keyword
import tokenize
import token
import token as f
if token is f:
    print("why")
if token == f:
    print("what")
L = f.tok_name
ALL_TOKEN_TYPES = token.tok_name  # {id: name}

# nice list of names
token_names = list(ALL_TOKEN_TYPES.values())
token_type = list(ALL_TOKEN_TYPES.keys())
for key in token_type:
    print(key)
print("tok")
for tok in token_names:
    print(tok)

print("")

OPERATORS = sorted(tokenize.EXACT_TOKEN_TYPES.keys())

for opp in OPERATORS:
    print(opp)

print("")

KEYWORDS = keyword.kwlist
for i in KEYWORDS:
    print(i)

SOFT_KEYWORDS = keyword.softkwlist
for i in SOFT_KEYWORDS:
    print(i)

print("")

python_lexical_info = {
    "token_types": list(token.tok_name.values()),
    "operators": sorted(tokenize.EXACT_TOKEN_TYPES.keys()),
    "keywords": keyword.kwlist,
    "soft_keywords": keyword.softkwlist,
}

for i in python_lexical_info:
    print(i)