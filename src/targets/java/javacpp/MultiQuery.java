package javacpp;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;

@Platform(include = {
        "REmatch/flags.hpp",
        "REmatch/multi_query.hpp",
        "REmatch/multi_match.hpp",
        "REmatch/span.hpp",
        "REmatch/multi_match_generator.hpp",
        "<vector>", "string"
}, link = {"REmatch", "stdc++"})
@Namespace("REmatch::library_interface")
@Name("MultiQuery")
public class MultiQuery extends Pointer {

    // static { System.loadLibrary("jniREmatch"); }
    static { JNILoader.load("jniREmatch"); }

    // Constructor para inicializar MultiQuery
    public MultiQuery(String pattern, @Cast("REmatch::library_interface::Flags") int flags,
                      int maxMempoolDuplications, int maxDeterministicStates) {
        allocate(pattern, flags, maxMempoolDuplications, maxDeterministicStates);
    }

    private native void allocate(String pattern, @Cast("REmatch::library_interface::Flags") int flags,
                                 int maxMempoolDuplications, int maxDeterministicStates);

    // Métodos nativos expuestos por MultiQuery
    public native @Cast("bool") boolean check(String document);

    public native @ByVal StringVector variables();

    public native @ByVal MultiMatch findone(String document);

    public native @ByVal MultiMatchVector findmany(@StdString String document, @Cast("uint_fast32_t") int limit);

    public native @ByVal MultiMatchVector findall(@StdString String document);

    public native @ByVal MultiMatchGenerator finditer(@StdString String document);

    public static void main(String[] args) {
        try {

            String pattern = "@(!extrasubdomain{\\w+}\\.)?!subdomains{\\w+}\\.!subdomains{\\w+}(\n|$)";
            Flags flags = Flags.NONE();
            int maxMempoolDuplications = 8;
            int maxDeterministicStates = 2000;

            MultiQuery multiQuery = new MultiQuery(pattern, flags.getVal(), maxMempoolDuplications, maxDeterministicStates);

            String document = "cperez@gmail.com\n" +
                                "soto@uc.cl\n" +
                                "sdelcampo@gmail.com\n" +
                                "lpalacios@gmeil.com\n" +
                                "pvergara@ing.uc.cl\n" +
                                "ndelafuente@ing.puc.cl\n" +
                                "ldelgado@gmsil.com\n" +
                                "tnovoa@mail.uc.cl\n" +
                                "nnarea@myucmail.uc.cl\n" +
                                "rramirez@gmail.com\n" +
                                "juansoto@uc.cl";

            System.out.println("Patrón de búsqueda: " + pattern);
            System.out.println("Documento a buscar: " + document + "\n");

            System.out.println("MULTIQUERY::CHECK()");
            boolean isValid = multiQuery.check(document);
            System.out.println("—— Documento es válido: " + isValid + "\n");

            System.out.println("MULTIQUERY::VARIABLES()");
            StringVector vars = multiQuery.variables();
            System.out.println("—— Variables encontradas en el match (desde MultiQuery): " + vars.size());
            for (long i = 0; i < vars.size(); i++) {
                System.out.println("—— Var "+i+": " + vars.at(i));
            }


            System.out.println("\nMULTIQUERY::FINDONE()");
            MultiMatch result = multiQuery.findone(document);

            int variableId = 1;
            System.out.println("—— MULTIMATCH::EMPTY()");
            System.out.println("————— MultiMatch está vacío: " + result.empty());
            System.out.println("—— MULTIMATCH::TO_STRING()");
            System.out.println("———— String del multiMatch: " + result.to_string());
            System.out.println("—— MULTIMATCH::SPANS()");
            long size = result.spans(variableId).size();
            System.out.println("————— Spans encontrados: " + result.spans(variableId).size());
            for (long i = 0; i < size; i++) {
                Span span = result.spans(variableId).at(i); // Obtener cada Span
                long first = span.first();
                long second = span.second();
                System.out.println("————— Span " + i + ": (" + first + ", " + second + ")");
            }
            System.out.println("—— MULTIMATCH::VARIABLES()");
            StringVector variables = result.variables();
            System.out.println("————— Variables encontradas en el multiMatch (desde MultiMatch): " + variables.size());
                for (long i = 0; i < variables.size(); i++) {
                    System.out.println("————— Var "+i+": " + variables.at(i));
                }
            System.out.println("—— MULTIMATCH::GROUPS()");
            StringVector groupResults = result.groups(variableId);
            StringVector groupResults2 = result.groups("subdomains");
            System.out.println("————— Grupos por variable_id " + "'"+variableId+"'" + ": " + groupResults);
            System.out.println("————— Grupos por variable_name 'subdomains': " + groupResults2);
            System.out.println("—— MULTIMATCH::SUBMATCH()");
            long start = result.spans(variableId).at(0).first();
            long end = result.spans(variableId).at(0).second();
            Span spanSubmatch = new Span(start, end);
            MultiMatch submatch = result.submatch(spanSubmatch); // Obtener el submatch para el intervalo especificado
            System.out.println("————— Submatch encontrado para la variable "+"'"+variableId+"':" + submatch.to_string());
            StringVector variablesSubmatch = submatch.groups(variableId);
            System.out.println("————— Variables encontradas en el submatch: " + variablesSubmatch.size());
            for (long i = 0; i < variablesSubmatch.size(); i++) {
                System.out.println("————— Variable " + i + ": " + variablesSubmatch.at(i));
            }
            System.out.println("—— MULTIMATCH::OPERATOREQUALS()");
            String document2 = "xyz.mail@otherdomain.com\n";
            String document3 = "cperez@gmail.com\n";
            String test_document1 = "cperez@gmail.com\n";
            MultiMatch match2 = multiQuery.findone(document2);
            MultiMatch match3 = multiQuery.findone(document3);
            System.out.println("————— match1 == match2: " + result.operatorEquals(match2)); // Esperado: false
            System.out.println("————— match1 == match3: " + result.operatorEquals(match3)); // Esperado: true
            System.out.println("————— match2 == match3: " + match2.operatorEquals(match3)); // Esperado: false            
            System.out.println("————— String de Match1: " + result.to_string());
            System.out.println("————— String de Match3: " + match3.to_string());   // por qué su operatorEquals es false?


            System.out.println("\nMULTIQUERY::FINDMANY()");
            int findNumber = 3;
            MultiMatchVector matches = multiQuery.findmany(document, findNumber);
            System.out.println("—— Matches encontradas para búsqueda de " + findNumber + ": " + matches.size());
            for (int i = 0; i < matches.size(); i++) {
                MultiMatch match = matches.at(i);
                System.out.println("————— " + i + ": " + match.to_string());
            }


            System.out.println("\nMULTIQUERY::FINDALL()");
            MultiMatchVector allMatches = multiQuery.findall(document);
            System.out.println("MultiMatches encontradas: " + allMatches.size());
            for (int i = 0; i < allMatches.size(); i++) {
                MultiMatch match = allMatches.at(i);
                System.out.println("————— " + i + ": " + match.to_string());
            }


            System.out.println("\nMULTIQUERY::FINDITER()");
            MultiMatchGenerator multiMatchGenerator = multiQuery.finditer(document);
            MultiMatchGeneratorIterator it = multiMatchGenerator.begin();
            System.out.println("Iterando multiMatches encontrados...");
            while (!it.operatorEquals(multiMatchGenerator.end())) {
                MultiMatch multiMatch = it.operator_star(); // Accede al objeto MultiMatch
                System.out.println(multiMatch.to_string()); 
                it.operator_increment();
            }

            System.out.println("\n");
            

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
 