package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

@Platform(include = {
        "REmatch/flags.hpp",
        "REmatch/query.hpp",
        "REmatch/match.hpp",
        "REmatch/span.hpp",
        "REmatch/match_generator.hpp",
        "<vector>", "string"
}, link = {"REmatch", "stdc++"})
@Namespace("REmatch::library_interface")
@Name("Query")
public class Query extends Pointer {
    
    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    
    // Constructor para inicializar Query
    public Query(String pattern, @Cast("REmatch::library_interface::Flags") int flags,
                 int maxMempoolDuplications, int maxDeterministicStates) {
        allocate(pattern, flags, maxMempoolDuplications, maxDeterministicStates);
    }

    private native void allocate(String pattern, @Cast("REmatch::library_interface::Flags") int flags,
                                 int maxMempoolDuplications, int maxDeterministicStates);

    // Métodos nativos expuestos por Query
    public native @Cast("bool") boolean check(String document);

    public native @ByVal StringVector variables();

    public native @ByVal Match findone(String document);

    public native @ByVal MatchVector findmany(@StdString String document, @Cast("uint_fast32_t") int limit);

    public native @ByVal MatchVector findall(@StdString String document);

    public native @ByVal MatchGenerator finditer(@StdString String document);

    public static void main(String[] args) {
        
        try {
            
            String pattern = "!x{aaa}";
            Flags flags = Flags.NONE();
            int maxMempoolDuplications = 8;
            int maxDeterministicStates = 2000;

            Query query = new Query(pattern, flags.getVal(), maxMempoolDuplications, maxDeterministicStates);

            String document = "aaaab";

            System.out.println("Patrón de búsqueda: " + pattern);
            System.out.println("Documento a buscar: " + document + "\n");

            boolean isValid = query.check(document);
            System.out.println("QUERY::CHECK()");
            System.out.println("—— Documento es válido: " + isValid + "\n");

            System.out.println("\nQUERY::VARIABLES()");
            StringVector vars = query.variables();
            System.out.println("——— Variables encontradas en el match (desde Query): " + vars.size());
            for (long i = 0; i < vars.size(); i++) {
                System.out.println("——— Var "+i+": " + vars.at(i));
            }

            System.out.println("\nQUERY::FINDONE()");
            Match result = query.findone(document);

            int variableIdFindone = 0;
            System.out.println("—— MATCH::EMPTY()");
            System.out.println("————— Match está vacío: " + result.empty());
            System.out.println("—— MATCH::TO_STRING()");
            System.out.println("————— String del match: " + result.to_string());
            System.out.println("—— MATCH::START()");
            System.out.println("————— Span de inicio por id: "+ result.start(variableIdFindone));
            System.out.println("————— Span de inicio por variable: "+ result.start("x"));
            System.out.println("—— MATCH::END()");
            System.out.println("————— Span de fin por id: "+ result.end(variableIdFindone));
            System.out.println("————— Span de fin por variable: "+ result.end("x"));
            System.out.println("—— MATCH::GROUP()");
            System.out.println("————— Grupo por id: " + result.group(variableIdFindone));
            System.out.println("————— Grupo por variable: " + result.group("x"));
            System.out.println("—— MATCH::SPAN()");
            System.out.println("————— Span por id (first, second): " + result.span(0).first() + ", " + result.span(0).second());
            System.out.println("————— Span por variable (first, second): " + result.span("x").first() + ", " + result.span("x").second());
            System.out.println("—— MATCH::GROUPDICT()");
            System.out.println("————— Diccionario de grupos por variable (first, second): " + result.groupdict().at("x").first() + ", " + result.groupdict().at("x").second());
            System.out.println("—— MATCH::VARIABLES()");
            StringVector vars_match = result.variables();
            System.out.println("————— Variables encontradas en el match (desde Match): " + vars_match.size());
            for (long i = 0; i < vars_match.size(); i++) {
                System.out.println("————— Var "+i+": " + vars_match.at(i));
            }

            System.out.println("\nQUERY::FINDMANY()");
            int findNumber = 3;
            MatchVector matches = query.findmany(document, findNumber);
            System.out.println("—— Matches encontradas para búsqueda de " + findNumber + ": " + matches.size());
            for (int i = 0; i < matches.size(); i++) {
                Match match = matches.at(i);
                System.out.println("————— " + i + ": " + match.to_string());
            }

            System.out.println("\nQUERY::FINDALL()");
            MatchVector allMatches = query.findall(document);
            System.out.println("Matches encontradas: " + allMatches.size());
            for (int i = 0; i < allMatches.size(); i++) {
                Match match = allMatches.at(i);
                System.out.println("————— " + i + ": " + match.to_string());
            }

            System.out.println("\nQUERY::FINDITER()");
            MatchGenerator matchGenerator = query.finditer(document);
            MatchGeneratorIterator it = matchGenerator.begin();
            System.out.println("Iterando matches encontrados...");
            int k = 0;
            while (!it.operatorEquals(matchGenerator.end())) {
                Match match = it.operator_star();
                System.out.println("————— " + k + ": " + match.to_string());
                it.operator_increment();
                k++;
            }

            System.out.println("\n");

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
